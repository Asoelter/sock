cmake_minimum_required (VERSION 3.10)

set(GLOBAL CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project ("client")

# Include sub-projects.
add_subdirectory ("source")
