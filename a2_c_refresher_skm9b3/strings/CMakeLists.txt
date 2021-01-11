cmake_minimum_required(VERSION 3.5)
project(strings)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Werror -g")
set(CMAKE_C_FLAGS "-std=c11 -Wall -Werror -g")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(strings_test test/tests.cpp src/sstring.c)
target_link_libraries(strings_test gtest pthread)

enable_testing()
add_test(NAME    strings_test
         COMMAND strings_test)


