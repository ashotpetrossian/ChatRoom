<<<<<<< HEAD
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <future>

class Client
{
public:
	Client(const std::string& host, const std::string& port)
		: host_{ host }, port_{port}, io_context_ { }, socket_{ io_context_ }, resolver_{ io_context_ }, isConnected_{ false }
	{ }

	~Client()
	{
		disconnect();
	}

	std::future<void> connect()
	{
		std::promise<void> promise;
		auto future = promise.get_future();

		try {
			boost::asio::connect(socket_, resolver_.resolve(host_, port_));
			isConnected_ = true;
			io_context_thread_ = std::thread([this]() { io_context_.run(); });
			startRead();
			promise.set_value();
		}
		catch (const std::exception& e) {
			promise.set_exception(std::make_exception_ptr(e));
		}

		return future;
	}

	void disconnect()
	{
		if (isConnected_) {
			isConnected_ = false;
			io_context_.stop();
			if (io_context_thread_.joinable()) {
				io_context_thread_.join();
			}

			boost::system::error_code ec;
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			socket_.close();
		}
	}

	void write(const std::string& message)
	{
		if (isConnected_) {
			boost::asio::async_write(socket_, boost::asio::buffer(message + '\0'), [](boost::system::error_code, std::size_t) { });
		}
	}

	void runWriteLoop()
	{
		std::string input;
		while (true) {
			std::cout << ":: ";
			std::getline(std::cin, input);

			input.erase(input.find_last_not_of(" \t\r\n") + 1);

			// If input is empty after trimming, ignore it
			if (input.empty()) {
				continue;
			}

			if (input == "exit") break;
			write(input);
		}

		disconnect();
	}

private:
	void startRead()
	{
		boost::asio::async_read_until(socket_, read_buffer_, '\0', [this](boost::system::error_code ec, std::size_t) {
			if (!ec) {
				std::string message{ std::istreambuf_iterator<char>(&read_buffer_), std::istreambuf_iterator<char>() };
				std::cout << "Server: " << message;
				read_buffer_.consume(message.size());
				startRead();
			}
			else {
				if (ec == boost::asio::error::eof) {
					std::cout << "Server disconnected" << std::endl;
				}
				else {
					std::cout << "Read error: " << ec.message() << std::endl;
				}

				disconnect();
			}
		});
	}

private:
	std::string host_;
	std::string port_;
	boost::asio::io_context io_context_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::streambuf read_buffer_;
	std::thread io_context_thread_;
	bool isConnected_;
};

int main()
{
	Client client("127.0.0.1", "25000");
	auto connectFuture = client.connect();
	try {
		connectFuture.get(); // blocks the thread until std::promise has set a value or exception
		client.runWriteLoop();
	}
	catch (const std::exception& e) {
		std::cout << "Connection error: " << e.what() << std::endl;
	}
=======
#include <boost/asio.hpp>
#include "Client.h"

Client::~Client()
{
	disconnect();
}

std::future<void> Client::connect()
{
	std::promise<void> promise;
	auto future = promise.get_future();

	try {
		boost::asio::connect(socket_, resolver_.resolve(host_, port_));
		isConnected_ = true;
		io_context_thread_ = std::thread([this]() { io_context_.run(); });
		startRead();
		promise.set_value();
	}
	catch (const std::exception& e) {
		promise.set_exception(std::make_exception_ptr(e));
	}

	return future;
}

void Client::disconnect()
{
	if (isConnected_) {
		isConnected_ = false;
		io_context_.stop();
		if (io_context_thread_.joinable()) {
			io_context_thread_.join();
		}

		boost::system::error_code ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		socket_.close();
	}
}

void Client::write(const std::string& message)
{
	if (isConnected_) {
		boost::asio::async_write(socket_, boost::asio::buffer(message + '\0'), [](boost::system::error_code, std::size_t) { });
	}
}

void Client::runWriteLoop()
{
	std::string input;
	while (true) {
		std::getline(std::cin, input);

		input.erase(input.find_last_not_of(" \t\r\n") + 1);

		// If input is empty after trimming, ignore it
		if (input.empty()) {
			continue;
		}

		if (input == "exit") break;
		write(input);
	}

	disconnect();
}

void Client::startRead()
{
	boost::asio::async_read_until(socket_, read_buffer_, '\0', [this](boost::system::error_code ec, std::size_t) {
		if (!ec) {
			std::string message{ std::istreambuf_iterator<char>(&read_buffer_), std::istreambuf_iterator<char>() };
			std::cout << message << std::endl;
			read_buffer_.consume(message.size());
			startRead();
		}
		else {
			if (ec == boost::asio::error::eof) {
				std::cout << "Server disconnected" << std::endl;
			}
			else {
				std::cout << "Read error: " << ec.message() << std::endl;
			}

			disconnect();
		}
	});
}

int main()
{
	Client client("192.168.10.22", "25000");
	//Client client("5.77.193.201", "25000");
	auto connectFuture = client.connect();
	try {
		connectFuture.get(); // blocks the thread until std::promise has set a value or exception
		client.runWriteLoop();
	}
	catch (const std::exception& e) {
		std::cout << "Connection error: " << e.what() << std::endl;
	}
>>>>>>> 40faee9 (Initial commit with existing files)
}