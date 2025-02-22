#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <future>

// Client class responsible for handling connections to the server
class Client
{
public:
    // Constructor initializes the client with the given host and port
    Client(const std::string& host, const std::string& port)
        : host_{ host }, port_{ port }, io_context_{ }, socket_{ io_context_ }, resolver_{ io_context_ }, isConnected_{ false }
    { }

    // Destructor ensures proper resource cleanup
    ~Client();

    // Initiates an asynchronous connection to the server
    std::future<void> connect();

    // Disconnects the client from the server
    void disconnect();

    // Sends a message to the server
    void write(const std::string& message);

    // Runs a loop to continuously send messages from user input
    void runWriteLoop();

private:
    // Starts reading responses from the server asynchronously
    void startRead();

private:
    std::string host_;  // Server hostname or IP address
    std::string port_;  // Server port
    boost::asio::io_context io_context_; // IO context for networking operations
    boost::asio::ip::tcp::socket socket_; // Socket for communication with the server
    boost::asio::ip::tcp::resolver resolver_; // Resolver to translate host and port into an endpoint
    boost::asio::streambuf read_buffer_; // Buffer to store incoming messages
    std::thread io_context_thread_; // Thread to run the IO context
    bool isConnected_; // Flag to track connection status
};
