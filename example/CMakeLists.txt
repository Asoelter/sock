cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("example")

add_subdirectory("client")
add_subdirectory("ngrep")
add_subdirectory("server")
