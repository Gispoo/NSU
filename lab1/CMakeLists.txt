cmake_minimum_required(VERSION 3.5.0)
project(lab1)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

add_executable(lab1 main.cpp
    src/ICacheable.cpp
    src/include/ICacheable.hpp
    src/LRUCache.cpp
    src/include/LRUCache.hpp
    src/LFUCache.cpp
    src/include/ReaLFUCacheder.hpp
    src/Fibonacci.cpp
    src/include/Fibonacci.hpp
)