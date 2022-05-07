cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)

add_library("network" "tcp_socket.h" "tcp_socket.cpp")
