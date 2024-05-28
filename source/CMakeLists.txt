set(LUX_LIB_TARGET_NAME lux_lib)
set(LUX_LIB_TARGET_NAME ${LUX_LIB_TARGET_NAME} PARENT_SCOPE)

set(LUX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/types/hash_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/hash_table.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/object.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/string.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types/value.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chunk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chunk.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scanner.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vm.hpp
)

add_library(${LUX_LIB_TARGET_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${LUX_SOURCES}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX source FILES ${LUX_SOURCES})

target_include_directories(${LUX_LIB_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(LUX_TARGET_NAME lux)

add_executable(${LUX_TARGET_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${LUX_TARGET_NAME} PRIVATE ${LUX_LIB_TARGET_NAME})
