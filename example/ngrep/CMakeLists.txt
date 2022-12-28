set(CMAKE_CXX_STANDARD 20)
set(GLOBAL CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("ngrep")

add_executable (${PROJECT_NAME} "main.cpp")

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror -g)

target_include_directories(${PROJECT_NAME} PUBLIC ../../)
target_include_directories(${PROJECT_NAME} PUBLIC nylon)
target_include_directories(${PROJECT_NAME} PUBLIC libngrep)
target_link_libraries(${PROJECT_NAME} nylon)
target_link_libraries(${PROJECT_NAME} libngrep)
