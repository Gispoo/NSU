cmake_minimum_required(VERSION 3.5.0)
project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(src/include exception/include)

add_executable(lab1 main.cpp
    src/ICache.cpp
    src/LRUCache.cpp
    src/LFUCache.cpp
    src/Fibonacci.cpp
    src/ArgumentReader.cpp
    exception/ELRUCache.cpp
    exception/ELFUCache.cpp
    exception/EFibonacci.cpp
    exception/EFibonacci.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(my_tests
    tests/ArgumentReaderTest.cpp
    tests/FibonacciTest.cpp
    tests/ICacheTest.cpp
    tests/LFUCacheTest.cpp
    tests/LRUCacheTest.cpp
)
target_link_libraries(my_tests gtest gtest_main)
add_test(CacherTests my_tests)

