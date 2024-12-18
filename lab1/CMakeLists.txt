cmake_minimum_required(VERSION 3.5.0)
project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(src/include exception/include tests)

add_executable(lab1 main.cpp
    src/ArgumentReader.cpp
    src/Fibonacci.cpp
    exception/EArgumentReader.cpp
    exception/EFibonacci.cpp
)

add_subdirectory(tests)