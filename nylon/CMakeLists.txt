cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)

project("nylon")

add_library("nylon"
            "nylon_message_builder.h"   "nylon_message_builder_inline.h"
            "nylon_client.h"            "nylon_client_inline.h"
            "nylon_server.h"            "nylon_server_inline.h"
            "nylon_file_writer.h"       "nylon_file_writer.cpp"
            )

add_subdirectory("network")
add_subdirectory("libngrep")

target_include_directories(${PROJECT_NAME} PRIVATE "libngrep")
target_link_libraries(${PROJECT_NAME} PRIVATE "network")
