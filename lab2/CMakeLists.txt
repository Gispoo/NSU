cmake_minimum_required(VERSION 3.28)
project(Nonogram)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/include)

add_executable(Nonogram main.cpp 
    src/menu.cpp
    src/grid.cpp
    src/nonogram.cpp
    src/game.cpp
    src/RunProgram.cpp
)

target_include_directories(Nonogram PRIVATE include)

find_package(raylib REQUIRED)

target_link_libraries(Nonogram PRIVATE raylib)

install(FILES include/*.hpp DESTINATION include)
