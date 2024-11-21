cmake_minimum_required(VERSION 3.5.0)
project(lab0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

add_executable(lab0 main.cpp
    src/CounterWord.cpp
    src/include/CounterWord.hpp
    src/Printer.cpp
    src/include/Printer.hpp
    src/Reader.cpp
    src/include/Reader.hpp
)