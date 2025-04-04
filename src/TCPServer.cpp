#include "../header/TCPServer.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // For inet_ntoa function

// Constructor to initialize server parameters (port and buffer size)
/*
server_fd(-1) initializes the server_fd (server socket file descriptor) to -1. This indicates that no socket has been created yet. A valid socket file descriptor will be assigned later when the socket is created.

client_fd(-1) initializes the client_fd (client socket file descriptor) to -1. It also indicates that no client connection has been established yet. It will be assigned when a client connects.

port(port) initializes the port member variable with the value passed as an argument to the constructor. This is the port number on which the server will listen for connections.

addrlen(sizeof(address)) initializes the addrlen variable with the size of the address structure. This is the length of the address structure that will be used when calling functions like accept().

*/
TcpServer::TcpServer(int port)
    : server_fd(-1), client_fd(-1), port(port), addrlen(sizeof(address)) {
    /*
    void* memset(void* ptr, int value, size_t num);
        ptr: A pointer to the block of memory you want to set. This can be a pointer to an array or a structure.
        value: The value to set. This is the byte value to be written to each byte of the memory. It is passed as an integer but will be cast to an unsigned char.
        num: The number of bytes to be set to the specified value.
    */
    memset(&address, 0, sizeof(address));  // Initialize the address structure to 0
    memset(buffer, 0, BUFFER_SIZE);        // Initialize buffer to 0
}

// Destructor to clean up open file descriptors (client and server sockets)
TcpServer::~TcpServer() {
    if (client_fd >= 0) close(client_fd);  // Close client socket if open
    if (server_fd >= 0) close(server_fd);  // Close server socket if open
    std::cout << "Server sockets closed.\n";
}

// Private method to create a socket
bool TcpServer::setupSocket() {
    // Create a socket (IPv4, stream-based TCP)
    /*
    AF_INET (Address Family):

        AF_INET stands for "Address Family - IPv4". 
        It specifies that the socket will be used for IPv4 communication, meaning it will handle Internet Protocol (IP) version 4 addresses (e.g., 192.168.1.1).
        The address family tells the system which kind of addresses the socket will use. In this case, AF_INET indicates that the socket will communicate using the IPv4 protocol.

    SOCK_STREAM (Socket Type):
        SOCK_STREAM indicates that the socket will be a stream socket, which corresponds to a TCP connection.
        TCP (Transmission Control Protocol) is a reliable, connection-oriented protocol. This type of socket is used when you want a continuous flow of data between two endpoints, such as a web server and a client.
        In contrast, SOCK_DGRAM is used for UDP (User Datagram Protocol), which is connectionless and doesn't guarantee delivery or order of data.

    0 (Protocol):
        The third parameter, 0, specifies the protocol to be used with the socket.
        A value of 0 means that the system should use the default protocol for the specified socket type (SOCK_STREAM in this case). For SOCK_STREAM, the default protocol is TCP.
        This argument is often set to 0 because it's generally implied by the socket type (SOCK_STREAM already indicates the use of TCP).
    */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return false;
    }

    // Set socket options to allow reusing address/port
    /*
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

        sockfd — the socket file descriptor (returned from socket()).

        level — specifies the protocol level:

            SOL_SOCKET means you're setting options at the socket level.

        optname — the specific option you’re setting:

            SO_REUSEADDR:
                Allows the reuse of a local address (IP and port).
                This is useful when the socket is in the TIME_WAIT state after being closed, and you want to restart the server without waiting.

            SO_REUSEPORT:
                Allows multiple sockets to bind to the same port, which is useful for load balancing (especially on multi-threaded or multi-process servers).


        optval — a pointer to the option value (in this case, &opt, which is 1).

        optlen — the size of the option value (sizeof(opt)).
        
        opt = 1: This means "enable" these options.

    */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return false;
    }

    // Set server address details (IPv4, any IP address, specific port)
    /*
    Together, these lines prepare the address struct to tell the OS:
    🗣️ "I want to accept IPv4 TCP connections on any IP bound to this machine, at a specific port number."
    */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any address
    address.sin_port = htons(port);        // Convert port to network byte order

    std::cout << "1- Socket setup complete.\n";

    return true;
}

// Private method to bind the socket to the address (IP and port)
/*

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
       
        sockfd:
        The socket file descriptor (from socket()), e.g., server_fd.

        addr:
        A pointer to a sockaddr structure containing the address info (IP, port).
        Here, you cast &address to (struct sockaddr*) to match the function signature.

        addrlen:
        Size of the address structure, usually sizeof(address).
*/
bool TcpServer::bindSocket() {
    // Bind server socket to the defined address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return false;
    }

    std::cout << "2- Socket binding complete " << std::endl;

    return true;
}

// Private method to listen for incoming client connections
/*
    int listen(int sockfd, int backlog);

        sockfd – the server socket file descriptor (from socket()).

        backlog – the maximum number of pending connections the system will allow in the queue before it starts rejecting new ones.

It just tells the OS: 🗣️ “I’m ready to accept connections.”
*/
bool TcpServer::listenSocket() {
    // Listen for incoming connections (with a backlog of 3 clients)
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return false;
    }

    std::cout << "3- Server listening on port: " << port << std::endl;

    return true;
}

// Private method to accept an incoming client connection
/*
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    ➤ What accept() does:
    Blocks and waits until a client tries to connect.

    Accepts the connection.

    Returns a new socket descriptor (client_fd) used to communicate with the connected client.

    The original server_fd continues listening for new connections.

*/
bool TcpServer::acceptClient() {
    // Accept an incoming connection and get the client socket
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_fd < 0) {
        perror("Accept failed");
        return false;
    }

    std::cout << "Client connected.\n";
    return true;
}

// Public method to start the server (handles all steps: setup, listen, accept)
/*

    This line uses the logical AND (&&) operator, which in C++ is short-circuiting.
    That means:

        setupSocket() is called first.

        Only if setupSocket() returns true, then bindSocket() is called.

        Then listenSocket(), and finally acceptClient() — each one depends on the one before.

*/
bool TcpServer::start() {
    return setupSocket() && bindSocket() && listenSocket() && acceptClient();
}

// Public method to send data to the connected client
bool TcpServer::sendData(const std::string& data) {
    if (client_fd < 0) 
    {
        std::cerr << "No client connected to server.\n";
        return false; // No client connected, return false
    }

    // Send data to the client (returns number of bytes sent)
    /*
    ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        Parameter	    Description
        client_fd	    The socket file descriptor for the connected client (from accept())
        data.c_str()	Pointer to the data (as a C-style string) to send
        data.size()	    Number of bytes to send
        0	            Flags (0 = no special options)
        ===============================================
        🧠 What does send(...) return?
            It returns the number of bytes successfully sent.
            If it returns -1, an error occurred.
    */
    int sent = send(client_fd, data.c_str(), data.size(), 0);
    return sent >= 0;  // Return true if data was sent successfully
}

// Public method to receive data from the connected client
std::string TcpServer::receiveData() {
    if (client_fd < 0) 
    {
        std::cerr << "No client connected to server.\n";
        return "No client connected to server\n"; // No client connected, return empty string
    }
    /*
    Clears the buffer by filling it with 0s to remove any leftover data from previous operations.
    This ensures we don't accidentally read old/stale data when we access buffer.
    */
    memset(buffer, 0, BUFFER_SIZE);  
    /*
        int bytesRead = read(client_fd, buffer, BUFFER_SIZE);
            Reads data from the client socket (client_fd) into buffer, up to BUFFER_SIZE bytes.

            read() is a system call. It will block (wait) until the client sends some data or disconnects.

            The return value bytesRead tells us how many bytes were actually received:

            If bytesRead > 0, some data was received.

            If bytesRead == 0, the client has closed the connection.

            If bytesRead < 0, an error occurred.
    */
    int bytesRead = read(client_fd, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) return "";  // If no data or error, return empty string

    /*
    std::string(buffer, bytesRead):

        buffer : is a character array (i.e., a C-style string).

        bytesRead : is the number of characters in that buffer that were actually filled with valid data from the client (not the whole buffer, just the part that was read).

        So, this constructor creates a C++ std::string from the first bytesRead characters of the buffer.

        summary : convert from C-style string to C++ string.
    */

    return std::string(buffer, bytesRead);  // Return the received data as a string
}
