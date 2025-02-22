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

// Represents a single client session
class Session : public std::enable_shared_from_this<Session>
{
public:
    // Constructor initializes session with a socket, session list, mutex, client ID, and reference to the server
    Session(boost::asio::ip::tcp::socket socket,
        std::vector<std::shared_ptr<Session>>& sessions,
        std::mutex& sessions_mutex,
        int id,
        Server& server);

    // Starts the session
    void run();

    // Sends a message to the client
    void sendMessage(const std::string& message);

    // Returns the session's unique client ID
    int getId() const { return id_; }

private:
    // Waits for incoming requests from the client
    void waitForRequest();

    // Cleans up the session when it ends
    void cleanUp();

private:
    boost::asio::ip::tcp::socket socket_; // Socket for communication with the client
    std::vector<std::shared_ptr<Session>>& sessions_; // Reference to the list of active sessions
    std::mutex& sessionsMutex_; // Mutex to synchronize access to sessions
    int id_; // Unique identifier for the client session
    Server& server_; // Reference to the main server instance
    boost::asio::streambuf buffer_; // Buffer for reading incoming data
};

// Server class that manages client connections and communication
class Server
{
public:
    // Constructor initializes the server with an IO context and a listening port
    Server(boost::asio::io_context& io_context, short port);

    // Destructor to handle cleanup
    ~Server();

    // Broadcasts a message to all clients except the sender
    void broadcast(int id, const std::string& message);

    // Returns the mutex used for synchronizing client thread access
    std::mutex& getClientThreadMutex() { return clientThreadsMutex_; }

    // Returns the mapping of client IDs to their respective threads
    std::unordered_map<int, std::thread>& getClientThreadsMapping() { return clientThreadsMapping_; }

    // Stops the server and cleans up resources
    void stop();

private:
    // Accepts incoming client connections
    void acceptClients();

private:
    boost::asio::io_context& io_context_; // Reference to the Boost ASIO IO context
    std::thread io_contextThread_; // Thread for running the IO context
    boost::asio::ip::tcp::acceptor acceptor_; // Acceptor object, which listens for incoming connections

    std::vector<std::shared_ptr<Session>> sessions_; // List of active client sessions
    std::unordered_map<int, std::string> clientMessages_; // Stores messages received from clients

    std::mutex sessionsMutex_; // Mutex to prevent race conditions when modifying the session list
    std::atomic<int> nextClientId_{ 0 }; // Atomic counter for assigning unique client IDs

    std::mutex clientMessagesMutex_; // Mutex for synchronizing access to client messages

    std::unordered_map<int, std::thread> clientThreadsMapping_; // Mapping of client IDs to threads handling them
    std::mutex clientThreadsMutex_; // Mutex for synchronizing access to client thread mapping
};
