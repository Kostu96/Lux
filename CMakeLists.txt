cmake_minimum_required(VERSION 3.16)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Lux LANGUAGES CXX)

add_subdirectory(third_party)
add_subdirectory(source)
add_subdirectory(tests)
