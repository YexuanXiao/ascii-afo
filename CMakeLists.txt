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

option(ASCII_AFO_BUILD_BENCHMARKS "Build Google Benchmark benchmarks" OFF)
if(ASCII_AFO_BUILD_BENCHMARKS)
    include(FetchContent)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.5)
    FetchContent_MakeAvailable(benchmark)

    add_executable(benchmark_ascii benchmark.cpp)
    target_link_libraries(benchmark_ascii PRIVATE benchmark::benchmark_main)
endif()
