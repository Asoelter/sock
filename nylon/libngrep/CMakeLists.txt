cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)

add_library("libngrep" INTERFACE)

target_include_directories(${PROJECT_NAME} PUBLIC ..)
target_link_libraries("libngrep" INTERFACE nylon)
