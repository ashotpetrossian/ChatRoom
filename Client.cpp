#include <boost/asio.hpp>
#include "Client.h"

Client::~Client()
{
	disconnect();
}

// Establishes a connection asynchronously and returns a future to indicate success or failure
std::future<void> Client::connect()
{
	// Create a promise to signal when the connection attempt completes
	std::promise<void> promise;
	// Retrieve a future associated with the promise, which will be returned
	auto future = promise.get_future();

	try {
		// Resolve the server address and establish a synchronous connection
		// This operation blocks the execution until the connection is fully established, 
		// meaning the program will not proceed to the next line of code until the 
		// connection process is complete.
		boost::asio::connect(socket_, resolver_.resolve(host_, port_));
		isConnected_ = true;

		// Starts the IO context in a separate thread to handle async operations
		io_context_thread_ = std::thread([this]() { io_context_.run(); });
		startRead();

		// Set promise to indicate successful connection
		promise.set_value();
	}
	catch (const std::exception& e) {
		// If an error occurs, store the exception in the promise
		promise.set_exception(std::make_exception_ptr(e));
	}

	// Return the future to allow the caller to wait for the connection result
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

		// Trim trailing whitespace
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

			// Consume the message from the buffer to remove processed data 
			// This prevents old messages from staying in the buffer and ensures 
			// that subsequent reads do not contain leftover data from previous messages.
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
	Client client("127.0.0.1", "25000");
	auto connectFuture = client.connect();
	try {
		connectFuture.get(); // blocks the thread until std::promise has set a value or exception, means until the connection attempt is resolved
		client.runWriteLoop();
	}
	catch (const std::exception& e) {
		std::cout << "Connection error: " << e.what() << std::endl;
	}
}
