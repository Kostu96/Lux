cmake_minimum_required(VERSION 3.16)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Lux LANGUAGES CXX)

set(LUX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/hash_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/hash_table.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/object.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/string.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/types/value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/chunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/chunk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/compiler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/scanner.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vm.hpp
)

add_executable(lux
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${LUX_SOURCES}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/source PREFIX source FILES ${LUX_SOURCES})

target_include_directories(lux PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
