cmake_minimum_required(VERSION 3.14)
add_definitions(-D_POSIX_C_SOURCE=1)
project(shell VERSION 1.0 DESCRIPTION "shell cmpt201." LANGUAGES C CXX)

# GoogleTest requires at least C++14

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
file(GLOB SOURCES src/*.c)
add_executable(shell ${SOURCES})

target_compile_options(shell PRIVATE -fsanitize=address -fsanitize=undefined)
target_link_options(shell PRIVATE -fsanitize=address -fsanitize=undefined)

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP false
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(shell_test gtest/shell_test.cpp)
target_link_libraries(shell_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(shell_test)
