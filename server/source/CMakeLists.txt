set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (${PROJECT_NAME} "main.cpp")

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror)

target_include_directories(${PROJECT_NAME} PUBLIC ../../)
target_link_libraries(${PROJECT_NAME} network)
