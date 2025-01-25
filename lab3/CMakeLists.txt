cmake_minimum_required(VERSION 3.5.0)
project(lab3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(src/include src/exception/include)

add_executable(lab3 main.cpp
    src/Menu.cpp
    src/MixerConverter.cpp
    src/SilenceConverter.cpp
    src/WavFile.cpp
    src/exception/WavFileException.cpp
    src/exception/SilenceConverterException.cpp
)
