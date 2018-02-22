cmake_minimum_required(VERSION 3.10)

add_executable(snake
    source/main.cpp)

target_link_libraries(snake pixeng)
