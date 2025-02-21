#include <boost/asio.hpp>
#include "Server.h"

Session::Session(boost::asio::ip::tcp::socket socket,
				std::vector<std::shared_ptr<Session>>& sessions,
				std::mutex& sessions_mutex,
				int id,
				Server& server)
	: socket_{ std::move(socket) },
	sessions_{ sessions },
	sessionsMutex_{ sessions_mutex },
	id_{ id },
	server_{ server }
{
}

void Session::run()
{
	std::cout << "Client connected with ID: " << id_ << std::endl;
	waitForRequest();
}

void Session::sendMessage(const std::string& message)
{
	boost::asio::async_write(socket_, boost::asio::buffer(message + '\0'), [](boost::system::error_code ec, std::size_t) {});
}


// Method to begin asynchronously reading data from the client
void Session::waitForRequest()
{
	// Creates a shared_ptr to the current Session object.
	// This is essential for ensuring that the Session object remains alive while the asynchronous operation is in progress.
	std::shared_ptr<Session> self{ shared_from_this() };

	// Asynchronous read operation - non-blocking operation. Immediately returns, allowing io_context to handle other events
	boost::asio::async_read_until(socket_, buffer_, "\0",
	[this, self](boost::system::error_code ec, std::size_t) {
			if (!ec) {
				std::string data{ std::istreambuf_iterator<char>(&buffer_), std::istreambuf_iterator<char>() };

				std::cout << "Client: " << id_ << ": " << data << std::endl;

				server_.broadcast(id_, data);
				waitForRequest();
			}
			else {
				if (ec == boost::asio::error::eof) {
					std::cout << "Client disconnected (ID: " << id_ << ") : " << socket_.remote_endpoint().address().to_string() << " : " << socket_.remote_endpoint().port() << std::endl;
				}
				else {
					std::cerr << "Error: " << ec.message() << std::endl;
				}

				cleanUp();
			}
		});
}

void Session::cleanUp()
{
	{
		std::lock_guard<std::mutex> lock{ sessionsMutex_ };
		sessions_.erase(std::remove(sessions_.begin(), sessions_.end(), shared_from_this()), sessions_.end());
	}

	{
		std::lock_guard<std::mutex> lock{ server_.getClientThreadMutex() };
		auto it = server_.getClientThreadsMapping().find(this->getId());
		if (it != server_.getClientThreadsMapping().end()) {
			if (it->second.joinable()) {
				it->second.join();
			}

			server_.getClientThreadsMapping().erase(it);
		}
	}
}

Server::Server(boost::asio::io_context& io_context, short port)
	:	io_context_{ io_context },
		acceptor_{io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)}
{
	std::cout << "Server started to listen to port: " << port << std::endl;
	std::cout << "Server local endpoint: " << acceptor_.local_endpoint().port() << std::endl;
	acceptClients();
}

Server::~Server()
{
	stop();
}

void Server::stop()
{
	acceptor_.close();
	io_context_.stop();

	if (io_contextThread_.joinable()) io_contextThread_.join();
	std::cout << "Server Stopped" << std::endl;

	{
		std::lock_guard<std::mutex> lock{ clientThreadsMutex_ };
		for (auto& [id, thread] : clientThreadsMapping_) {
			if (thread.joinable()) {
				thread.join();
			}
		}

		clientThreadsMapping_.clear();
	}

	{
		std::lock_guard<std::mutex> lock{ sessionsMutex_ };
		sessions_.clear();
	}

	std::cout << "Server stopped" << std::endl;
}

void Server::broadcast(int senderId, const std::string& message)
{
	std::lock_guard<std::mutex> lock{ clientMessagesMutex_ };
	clientMessages_[senderId].append(std::to_string(senderId) + ": " + message + '\n');

	for (const auto& session : sessions_) {
		if (session->getId() != senderId)
		session->sendMessage(message + '\n');
	}
}

void Server::acceptClients()
{
	acceptor_.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
		if (!ec) {
			std::cout << "Creating session on: " << socket.remote_endpoint().address().to_string() << ": remote endpoint: "
				<< socket.remote_endpoint().port() << " =, local endpoint: " << socket.local_endpoint().port() << std::endl;

			int clientId{ nextClientId_++ };
			std::shared_ptr<Session> session = std::make_shared<Session>(std::move(socket), sessions_, sessionsMutex_, clientId, *this);
			{
				std::lock_guard<std::mutex> lock{ sessionsMutex_ };
				sessions_.push_back(session);
			}

			std::thread clientThread([session]() {
				session->run();
			});

			{
				std::lock_guard<std::mutex> lock{ clientThreadsMutex_ };
				clientThreadsMapping_[clientId] = std::move(clientThread);
			}
		}
		else {
			std::cerr << "Error: " << ec << std::endl;
		}

		acceptClients();
	});
}

int main()
{
	boost::asio::io_context io_context;
	Server server(io_context, 25000);
	std::thread io_context_thread([&io_context]() { io_context.run(); });

	boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
	signals.async_wait([&io_context, &server](boost::system::error_code ec, int signalNumber) {
		if (!ec) {
			std::cout << "Signal received: " << signalNumber << ". Stopping server..." << std::endl;
			server.stop(); // Call the server's stop method
		}
	});

	io_context.run();
}