#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <string>
#include <netinet/in.h>

// TcpServer class defines the server-side functionality for a TCP connection
class TcpServer {
public:
    // Constructor to initialize server with a specified port
    TcpServer(int port);

    // Destructor to clean up any open resources (like sockets)
    ~TcpServer();

    // Public method to start the server, including setup and client connection
    bool start();

    // Method to send data to the client
    bool sendData(const std::string& data);

    // Method to receive data from the client
    std::string receiveData();

private:
    // File descriptors for server and client
    int server_fd;
    int client_fd;

    // Port number for the server to listen on
    int port;

    // Address structure to store server address info
    struct sockaddr_in address;

    // Size of the address structure
    int addrlen;

    // Buffer size for receiving data
    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    // Private helper methods for setting up the server
    bool setupSocket();
    bool bindSocket();
    bool listenSocket();
    bool acceptClient();
};

#endif
