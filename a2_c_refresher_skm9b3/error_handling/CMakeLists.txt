cmake_minimum_required(VERSION 3.5)
project(error_handling)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Werror -g")
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -g")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(error_handling_test test/tests.cpp src/error_handling.c)
target_link_libraries(error_handling_test gtest pthread)

enable_testing()
add_test(NAME    error_handling_test
         COMMAND error_handling_test)

file(COPY test/records.bin DESTINATION .)
