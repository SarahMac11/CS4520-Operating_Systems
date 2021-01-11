cmake_minimum_required(VERSION 3.5)
project(arrays)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Werror -g")
set(CMAKE_C_FLAGS "-std=c11 -Wall -Werror -g")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(arrays_test test/tests.cpp src/arrays.c)
target_link_libraries(arrays_test gtest pthread)

enable_testing()
add_test(NAME    arrays_test
         COMMAND arrays_test)


