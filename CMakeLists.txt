cmake_minimum_required(VERSION 3.0)
project(nbashes C)

set(CMAKE_C_STANDARD 99)

add_executable(nbashes main.c)
target_link_libraries(nbashes ncursesw)
