set(LUX_TESTS_TARGET_NAME lux_tests)

set(LUX_TESTS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/error_output_tests.cpp
)

add_executable(${LUX_TESTS_TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${LUX_TESTS_SOURCES}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX tests FILES ${LUX_TESTS_SOURCES})

target_link_libraries(${LUX_TESTS_TARGET_NAME} PRIVATE
	${LUX_LIB_TARGET_NAME}
	gtest
	gtest_main
)
