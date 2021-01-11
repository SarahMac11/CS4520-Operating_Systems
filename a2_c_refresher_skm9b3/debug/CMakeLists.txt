cmake_minimum_required(VERSION 3.5)
project(debug)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Werror -g")
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -g")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(debug_test test/tests.cpp src/debug.c)
target_link_libraries(debug_test gtest pthread)

enable_testing()
add_test(NAME    debug_test
         COMMAND debug_test)


