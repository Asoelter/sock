set(CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (${PROJECT_NAME} "main.cpp")

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror -g)

target_include_directories(${PROJECT_NAME} PUBLIC ../../)
target_include_directories(${PROJECT_NAME} PUBLIC network)
target_link_libraries(${PROJECT_NAME} network)

target_link_libraries(${PROJECT_NAME} nylon)
