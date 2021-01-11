cmake_minimum_required (VERSION 3.5)
project(structures)

set(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -Werror")
set(CMAKE_C_FLAGS "-std=c11 -g -Wall -Wextra")

# Link runTests with what we want to test and the GTest and pthread library
add_executable(${PROJECT_NAME}_tester
               test/test.cpp
               src/${PROJECT_NAME}.c)

target_link_libraries(${PROJECT_NAME}_tester gtest pthread)

enable_testing()
add_test(NAME    ${PROJECT_NAME}_test
         COMMAND ${PROJECT_NAME}_test)
