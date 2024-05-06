cmake_minimum_required(VERSION 3.29.0)
project(TopSort VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(TopSort main.c
    TopSort/top_sort.c
    TopSort/top_sort.h
    Errors/errors.c
    Errors/errors.h
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
