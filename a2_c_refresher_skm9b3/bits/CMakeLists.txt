cmake_minimum_required(VERSION 3.5)
project(bits)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Werror -g")
set(CMAKE_C_FLAGS "-Wall -Werror -g")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(bitmap_test test/tests.cpp src/bitmap.c)
target_link_libraries(bitmap_test gtest pthread)

enable_testing()
add_test(NAME    bitmap_test
         COMMAND bitmap_test)

