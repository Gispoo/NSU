cmake_minimum_required(VERSION 3.29.0)
project(Huffman VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(Huffman main.c 
    Decode/decode.c
    Decode/decode.h
    Encode/encode.c
    Encode/encode.h
    Errors/errors.c
    Errors/errors.h
    huffman.c
    huffman.h
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)