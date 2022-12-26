cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("sock")

add_subdirectory("client")
add_subdirectory("network")
add_subdirectory("nylon")
add_subdirectory("server")
add_subdirectory("tests")
