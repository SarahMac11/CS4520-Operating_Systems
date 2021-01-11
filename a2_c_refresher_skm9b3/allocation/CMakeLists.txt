cmake_minimum_required (VERSION 3.5)
project(allocation)

set(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -Werror")
set(CMAKE_C_FLAGS "-std=c11 -g -Wall -Wextra")

add_executable(${PROJECT_NAME}_tester test/tests.cpp src/${PROJECT_NAME}.c)
target_link_libraries(${PROJECT_NAME}_tester gtest pthread)
