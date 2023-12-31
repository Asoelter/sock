set(CMAKE_CXX_STANDARD 20)

set(TEST_TARGET "test")

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

#For windows: Prevent override the parent object's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL " " FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(${TEST_TARGET} "test.cpp")

target_compile_options(${TEST_TARGET} PUBLIC -Wall -Wextra -Wpedantic -Werror -g)

target_include_directories(${TEST_TARGET} PUBLIC ../)
target_include_directories(${TEST_TARGET} PUBLIC nylon)
target_include_directories(${TEST_TARGET} PUBLIC libngrep)
target_link_libraries(${TEST_TARGET} GTest::gtest_main)
target_link_libraries(${TEST_TARGET} "nylon")
target_link_libraries(${TEST_TARGET} "libngrep")
include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})
