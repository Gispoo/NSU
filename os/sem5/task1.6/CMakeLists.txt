cmake_minimum_required(VERSION 3.10)

project(MyThread)

# Устанавливаем стандарт C для компиляции
set(CMAKE_C_STANDARD 11)

# Создание динамической библиотеки mythread
add_library(mythread SHARED mythread/mythread.c)

# Устанавливаем include директорию для библиотеки
target_include_directories(mythread PUBLIC ${CMAKE_SOURCE_DIR}/mythread)

add_executable(main.out src/main.c)

# Связываем исполняемый файл с динамической библиотекой mythread
target_link_libraries(main.out PRIVATE mythread)

# Установка пути к сборке для удобного запуска main
set_target_properties(main.out PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(mythread PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Установка RPATH для поиска динамических библиотек
set_target_properties(main.out PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
