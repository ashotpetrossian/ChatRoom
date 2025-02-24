# ChatRoom
Project Overview: Multi-Client Chat Server and Client using Boost.Asio
This project demonstrates a multi-client chat application where multiple clients can connect to a central server, send and receive messages asynchronously, and broadcast messages to all connected clients. The project is built using Boost.Asio, which provides a powerful and efficient way to handle asynchronous I/O operations in C++. This is an excellent example of using asynchronous programming and multithreading to build a real-time communication system.

Components
The system consists of two major components: the server and the client.

## 1. Server (Server.cpp & Server.h)
The server is responsible for accepting incoming client connections and handling the communication between clients. The server can handle multiple client sessions at the same time and broadcasts messages sent by any client to all other connected clients.

### Key Classes:
#### Session:

* Each Session represents a single client connected to the server.
* Handles reading messages from the client asynchronously using boost::asio::async_read_until to allow the server to perform other tasks while waiting for client input.
* Once a message is received, the server will broadcast it to all other connected clients.
* If the client disconnects or an error occurs, the session is cleaned up, and the client is removed from the list of active sessions.

#### Server:

* The Server is responsible for accepting new client connections and managing the active sessions.
* The server maintains a list of all active client sessions and uses the boost::asio::ip::tcp::acceptor to listen for new incoming connections.
* For each new client connection, a Session object is created, and the server starts handling that client in a separate thread.
* It also broadcasts messages from one client to all other clients connected to the server.

#### Key Features:
* Multi-Client Support: The server is capable of handling multiple client connections concurrently.
* Broadcasting: Messages sent by a client are broadcasted to all other clients.
* Asynchronous Communication: Both reading from clients and writing to them are done asynchronously, allowing the server to process multiple tasks concurrently without blocking.
* Client Disconnect Handling: When a client disconnects, the server cleans up resources, updates the list of connected clients, and notifies other clients.


## 2. Client (Client.cpp & Client.h)
* The client connects to the server, sends messages, and receives messages. The client continuously listens for user input, sends it to the server, and prints any messages received from the server.

### Key Classes:

#### Client:
* Handles the connection to the server via the boost::asio::ip::tcp::socket.
* Establishes an asynchronous connection to the server using boost::asio::connect.
* Sends messages to the server using boost::asio::async_write.
* Reads messages from the server asynchronously using boost::asio::async_read_until.
* The runWriteLoop function continuously listens for user input and sends it to the server until the user types "exit".
* The client also listens for incoming messages from the server and prints them to the console.

#### Key Features:
* Asynchronous Communication: The client uses asynchronous reads and writes, meaning it can send and receive messages without blocking.
* User Input Loop: The client continuously waits for user input and sends it to the server.
* Graceful Disconnect: The client has the ability to gracefully disconnect from the server when the user types "exit".


### How It Works:
#### Server Initialization:
* The server starts by binding to a specific port and begins accepting incoming client connections.
* For each new client, a new Session is created and added to the list of active sessions.
* The server listens for incoming messages from clients, and when a message is received, it broadcasts that message to all other connected clients.

#### Client Initialization:
* Each client connects to the server by specifying the server's IP address and port number.
* Once connected, the client starts listening for messages from the server and waits for user input to send messages to the server.

#### Communication:
* Clients can send messages to the server, which are then broadcasted to all other clients.
* Messages are sent asynchronously using async_write, and the server reads the messages asynchronously using async_read_until.
* The client receives messages from the server asynchronously and displays them on the console.

#### Concurrency:
* The server can handle multiple clients concurrently by creating a new Session for each client and using asynchronous I/O operations.
* Each client can read from and write to the server asynchronously, enabling non-blocking operations.
* The server uses multiple threads to handle client connections, and each session operates in its own thread.

#### Disconnect and Cleanup:
* When a client disconnects, the session object is cleaned up, and the client is removed from the active sessions.
* The server notifies all other clients that a user has disconnected.


### Features and Extensions:
* Private Messaging: The server could be extended to support private messaging between clients, where a message can be sent only to a specific client rather than broadcasting to all clients.
* User Authentication: A feature could be added where clients need to log in with a username and password before they can send messages.
* Message History: The server could store past messages and allow clients to request a message history.
* Error Handling: Better error handling and reconnection logic could be added to deal with network failures or disconnections.
* GUI Client: The current implementation is a console-based client, but it could be extended to include a GUI for better user interaction.

### Project Benefits:
* Real-Time Communication: This project demonstrates real-time communication between clients using asynchronous networking.
* Multithreading: By using threads for handling multiple clients and asynchronous I/O, the server can scale efficiently to support many simultaneous connections.
* Learning Asynchronous Programming: By using Boost.Asio's asynchronous operations, developers can learn how to handle I/O operations without blocking the main thread, which is a crucial skill for real-time applications.

