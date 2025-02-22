#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <future>

class Client
{
public:
	Client(const std::string& host, const std::string& port)
		: host_{ host }, port_{ port }, io_context_{ }, socket_{ io_context_ }, resolver_{ io_context_ }, isConnected_{ false }
	{
	}

	~Client();

	std::future<void> connect();
	void disconnect();

	void write(const std::string& message);
	void runWriteLoop();

private:
	void startRead();

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