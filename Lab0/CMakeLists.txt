cmake_minimum_required(VERSION 3.5.0)
project(lab0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

add_executable(lab0 main.cpp
    src/CountWords.cpp
    src/include/CountWords.hpp
    src/Print.cpp
    src/include/Print.hpp
    src/Start.cpp
    src/include/Start.hpp
)