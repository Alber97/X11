cmake_minimum_required(VERSION 3.0)
project(X11 C)

set(CMAKE_C_STANDARD 11)

include_directories("/usr/include/X11")

find_package(X11 REQUIRED)

add_executable(X11 main.c)

link_directories("/usr/lib")
target_link_libraries(X11Example X11)