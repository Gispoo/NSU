cmake_minimum_required(VERSION 3.10)
project(TestLab1)

find_package(GTest REQUIRED)

include_directories(../src/include ../exception)

add_executable(TestLab1 main_test.cpp
    ./src/LRUCacheTest.cpp
    ./src/LFUCacheTest.cpp
)

target_link_libraries(TestLab1 gtest gtest_main)