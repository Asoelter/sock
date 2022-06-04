cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)

project("nylon")

add_library("nylon"
            "nylon_message.h"           "nylon_message.cpp"
            "nylon_client.h"            "nylon_client.cpp"
            )

target_link_libraries(${PROJECT_NAME} PRIVATE "network")
