project(Tests)

file(GLOB_RECURSE TESTS_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE TESTS_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

foreach(TEST_SOURCE ${TESTS_SOURCES})
	get_filename_component(TEST_SOURCE_WE ${TEST_SOURCE} NAME_WE)
	add_executable(${TEST_SOURCE_WE} ${TEST_SOURCE} ${TESTS_HEADERS})
	target_link_libraries(${TEST_SOURCE_WE} PRIVATE Engine)

	set_target_properties(${TEST_SOURCE_WE} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
	target_output_properties(${TEST_SOURCE_WE} ${TEST_SOURCE_WE} ${BLOODSHOT_BINARIES_DIR})

	add_test(NAME ${TEST_SOURCE_WE} COMMAND ${TEST_SOURCE_WE})
	add_dependencies(${TEST_SOURCE_WE} Engine)
endforeach()
