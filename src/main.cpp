#include "../header/TCPServer.hpp"
#include <iostream>
#include <cstdlib>  // For system()
#include <algorithm>  // For std::remove
#include <string>     // For std::string
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void toLowerCase(std::string& clientMsg) {
    std::transform(clientMsg.begin(), clientMsg.end(), clientMsg.begin(), ::tolower);
}

void openFacebookLinux() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process: Launch the browser
        execlp("google-chrome", "google-chrome",
               "--user-data-dir=/tmp/fb_session",
               "--app=https://facebook.com",
               "--kiosk",
               "--disable-gpu",               // Optional: helps reduce GPU warnings
               (char *)NULL);

        // If execlp fails
        std::cerr << "Failed to launch browser\n";
        exit(1);
    } else if (pid < 0) {
        std::cerr << "Fork failed\n";
    } else {
        std::cout << "Facebook should be launching in the background.\n";
    }
}

void openYoutubeLinux() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process: Launch the browser
        execlp("google-chrome", "google-chrome",
               "--user-data-dir=/tmp/youtube_session",  // You can change the session directory
               "--app=https://youtube.com",             // Change URL to YouTube
               "--kiosk",
               "--disable-gpu",                        // Optional: helps reduce GPU warnings
               (char *)NULL);

        // If execlp fails
        std::cerr << "Failed to launch browser\n";
        exit(1);
    } else if (pid < 0) {
        std::cerr << "Fork failed\n";
    } else {
        std::cout << "YouTube should be launching in the background.\n";
    }
}

void openGitHUbLInux()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        // Child process: Launch the browser
        execlp("google-chrome", "google-chrome",
               "--user-data-dir=/tmp/github_session",  // You can change the session directory
               "--app=https://github.com",             // Change URL to GitHub
               "--kiosk",
               "--disable-gpu",                        // Optional: helps reduce GPU warnings
               (char *)NULL);

        // If execlp fails
        std::cerr << "Failed to launch browser\n";
        exit(1);

    }
    else if (pid < 0)
    {
        std::cerr << "Fork failed\n";
    }
    else
    {
        std::cout << "GitHub should be launching in the background.\n";
    }
}



// Function to increase the volume by 5%
void increaseVolume() {
    std::cout << "Increasing volume..." << std::endl;
    system("xdotool key XF86AudioRaiseVolume");
}

void decreaseVolume() {
    std::cout << "Decreasing volume..." << std::endl;
    system("xdotool key XF86AudioLowerVolume");
}


int main() {
    // Create the server instance, listening on port 8080
    TcpServer server(8080);

    // Start the server (sets up socket, binds, listens, and accepts client)
    if (!server.start()) {
        std::cerr << "Failed to start server.\n";
        return 1;
    }

    bool youtubeMode = false;

    while (true) {
        // Receive data from the client
        std::string clientMsg = server.receiveData();

        // Clean up newline and carriage return characters from client message
        clientMsg.erase(std::remove(clientMsg.begin(), clientMsg.end(), '\n'), clientMsg.end());
        clientMsg.erase(std::remove(clientMsg.begin(), clientMsg.end(), '\r'), clientMsg.end());

        std::cout << "Client: " << clientMsg << std::endl;

        // Convert the message to lowercase to handle case-insensitive input
        toLowerCase(clientMsg);

        if (clientMsg == "open facebook") {
            std::cout << "Opening Facebook..." << std::endl;

            #ifdef _WIN32
                system("start https://www.facebook.com");
            #elif __APPLE__
                system("open https://www.facebook.com");
            #elif __linux__
                openFacebookLinux();
            #endif
        }
        else if (clientMsg == "close facebook") {
            std::cout << "Closing Facebook..." << std::endl;

            #ifdef __linux__
                system("pkill -f '/tmp/fb_session'");
            #endif
        }
        else if (clientMsg == "open youtube") {
            std::cout << "Opening Youtube..." << std::endl;

            #ifdef _WIN32
                system("start https://www.Youtube.com");
            #elif __APPLE__
                system("open https://www.Youtube.com");
            #elif __linux__
                openYoutubeLinux();
            #endif
            youtubeMode = true;
            server.sendData("YouTube opened. You can now search songs using: play <song name>\n");
        }
        else if (clientMsg == "close youtube") {
            std::cout << "Closing Youtube..." << std::endl;

            #ifdef __linux__
                system("pkill -f '/tmp/youtube_session'");
            #endif
        }
        else if (clientMsg == "vol+") {
            increaseVolume();
        }
        else if (clientMsg == "vol-") {
            decreaseVolume();
        }
        else if (clientMsg == "open github")
        {
            std::cout << "Opening GitHub..." << std::endl;

            #ifdef _WIN32
                system("start https://www.github.com");
            #elif __APPLE__
                system("open https://www.github.com");
            #elif __linux__
                openGitHUbLInux();
            #endif
        }
        else if (clientMsg == "close github")
        {
            std::cout << "Closing GitHub..." << std::endl;

            #ifdef _WIN32
                system("taskkill /F /IM chrome.exe");
            #elif __APPLE__
                system("pkill -f 'Google Chrome'");
            #elif __linux__
                system("pkill -f '/tmp/github_session'");
            #endif
        }
        else if (clientMsg == "exit") {
            std::cout << "Shutting down server." << std::endl;
            break; // Exit the loop and end the server
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }

        // Send a response back to the client
        server.sendData("Command received.\n");
    }

    return 0; // Destructor will automatically close the sockets when exiting
}
