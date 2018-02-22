cmake_minimum_required(VERSION 3.10)

set(LIB_NAME pixeng)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)

add_library(${LIB_NAME} SHARED
    source/pixeng.cpp)

target_include_directories(${LIB_NAME} PUBLIC include)
target_include_directories(${LIB_NAME} PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(${LIB_NAME} ${SDL2_LIBRARY})
