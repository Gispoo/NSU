cmake_minimum_required(VERSION 3.29.0)
project(Prim VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(Prim main.c
    Prim/prim.c
    Prim/prim.h
    Errors/errors.c
    Errors/errors.h
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
