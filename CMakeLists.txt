cmake_minimum_required(VERSION 3.29)

project(ascii_afo)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(examples examples.cpp)

enable_testing()
add_executable(test_ascii tests/test_ascii.cpp)
target_include_directories(test_ascii PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME test_ascii COMMAND test_ascii)
