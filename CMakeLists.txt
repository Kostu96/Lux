cmake_minimum_required(VERSION 3.16)

project(Lux LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lux
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/source/chunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/chunk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/compiler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/scanner.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vm.hpp
)
