<<<<<<< HEAD
#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <unordered_map>

class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::ip::tcp::socket socket,
			std::vector<std::shared_ptr<Session>>& sessions,
			std::mutex& sessions_mutex,
			int id,
			Server& server);

	void run();
	void sendMessage(const std::string& message);
	int getId() const { return id_; }
	
private:
	void waitForRequest();
	void cleanUp();
	
private:
	boost::asio::ip::tcp::socket socket_;
	std::vector<std::shared_ptr<Session>>& sessions_;
	std::mutex& sessionsMutex_;
	int id_;
	Server& server_;
	boost::asio::streambuf buffer_;
};

class Server
{
public:
	Server(boost::asio::io_context& io_context, short port);
	~Server();
	void broadcast(int id, const std::string& message);
	std::mutex& getClientThreadMutex() { return clientThreadsMutex_; }
	std::unordered_map<int, std::thread>& getClientThreadsMapping() { return clientThreadsMapping_; }

	void stop();
private:
	void acceptClients();
private:
	boost::asio::io_context& io_context_;
	std::thread io_contextThread_;
	boost::asio::ip::tcp::acceptor acceptor_; // Acceptor object, which listens for incoming connections
	std::vector<std::shared_ptr<Session>> sessions_;
	std::unordered_map<int, std::string> clientMessages_;

	std::mutex sessionsMutex_; // to resolve data race while adding multiple clients to the m_sessions
	std::atomic<int> nextClientId_{ 0 };
	std::mutex clientMessagesMutex_;

	std::unordered_map<int, std::thread> clientThreadsMapping_;
	std::mutex clientThreadsMutex_;
=======
#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <unordered_map>

class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::ip::tcp::socket socket,
			std::vector<std::shared_ptr<Session>>& sessions,
			std::mutex& sessions_mutex,
			int id,
			Server& server);

	void run();
	void sendMessage(const std::string& message);
	int getId() const { return id_; }
	
private:
	void waitForRequest();
	void cleanUp();
	
private:
	boost::asio::ip::tcp::socket socket_;
	std::vector<std::shared_ptr<Session>>& sessions_;
	std::mutex& sessionsMutex_;
	int id_;
	Server& server_;
	boost::asio::streambuf buffer_;
};

class Server
{
public:
	Server(boost::asio::io_context& io_context, short port);
	~Server();
	void broadcast(int id, const std::string& message);
	std::mutex& getClientThreadMutex() { return clientThreadsMutex_; }
	std::unordered_map<int, std::thread>& getClientThreadsMapping() { return clientThreadsMapping_; }

	void stop();
private:
	void acceptClients();
private:
	boost::asio::io_context& io_context_;
	std::thread io_contextThread_;
	boost::asio::ip::tcp::acceptor acceptor_; // Acceptor object, which listens for incoming connections
	std::vector<std::shared_ptr<Session>> sessions_;
	std::unordered_map<int, std::string> clientMessages_;

	std::mutex sessionsMutex_; // to resolve data race while adding multiple clients to the m_sessions
	std::atomic<int> nextClientId_{ 0 };
	std::mutex clientMessagesMutex_;

	std::unordered_map<int, std::thread> clientThreadsMapping_;
	std::mutex clientThreadsMutex_;
>>>>>>> 40faee9 (Initial commit with existing files)
};