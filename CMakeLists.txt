cmake_minimum_required(VERSION 3.16)

project(Lux LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lux
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/source/chunk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)
