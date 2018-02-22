cmake_minimum_required(VERSION 3.10)

add_executable(cellular_automata
    source/main.cpp)

target_link_libraries(cellular_automata pixeng)
