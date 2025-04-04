#include <iostream>              // For input/output
#include <unistd.h>              // For read(), write(), close()
#include <string.h>              // For memset(), strlen()
#include <sys/socket.h>          // For socket functions
#include <arpa/inet.h>           // For inet_pton()
#include <netinet/in.h>          // For sockaddr_in structure

#define PORT 8080                // Must match the server port

int main() {
    int sock = 0;                           // Socket file descriptor
    struct sockaddr_in serv_addr;          // Server address structure
    const char *message = "Hello from client";  // Message to send
    char buffer[1024] = {0};               // Buffer to receive server response

    // 1. Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    // 2. Set server address info
    serv_addr.sin_family = AF_INET;             // IPv4
    serv_addr.sin_port = htons(PORT);           // Port in network byte order

    // 3. Convert IPv4 address from text to binary (127.0.0.1 = localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    // 4. Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    // 5. Send message to server
    send(sock, message, strlen(message), 0);
    std::cout << "Message sent to server" << std::endl;

    // 6. Read server's response
    read(sock, buffer, 1024);
    std::cout << "Server says: " << buffer << std::endl;

    // 7. Close the socket
    close(sock);

    return 0;
}
