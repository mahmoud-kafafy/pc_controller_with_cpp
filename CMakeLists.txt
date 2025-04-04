# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(TCPServerApp VERSION 1.0)

# Set C++ standard to use
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories (add the 'include' folder if needed)
include_directories(${PROJECT_SOURCE_DIR}/header)

# Add the executable target
add_executable(tcp_server src/TCPServer.cpp src/main.cpp)

# Optionally, you can set any flags here
# Example: set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
