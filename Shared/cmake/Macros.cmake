macro(get_paths_by_pattern RESULT TARGET_DIR PATTERN ANTIPATTERN)
	set(TEMP)
	list(APPEND DIRS_TO_OBSERVE ${TARGET_DIR})
	list(LENGTH DIRS_TO_OBSERVE REMAINED)
	while(REMAINED)
		foreach(DIR ${DIRS_TO_OBSERVE})
			file(GLOB CONTENT RELATIVE ${DIR} ${DIR}/*)
			foreach(PART ${CONTENT})
				if(IS_DIRECTORY ${DIR}/${PART})
					if(${PART} STREQUAL ${PATTERN})
						file(GLOB_RECURSE _IN_PATTERN RELATIVE ${DIR}/${PART} ${DIR}/${PART}/*.h)
						list(LENGTH _IN_PATTERN CONTAINS)
						if(CONTAINS)
							list(APPEND TEMP ${DIR}/${PART})
						endif()
					elseif(NOT ${PART} STREQUAL ${ANTIPATTERN})
						list(APPEND DIRS_TO_OBSERVE ${DIR}/${PART})
					endif()
				endif()
			endforeach()
			list(REMOVE_ITEM DIRS_TO_OBSERVE ${DIR})
		endforeach()
		list(LENGTH DIRS_TO_OBSERVE REMAINED)
	endwhile()
	set(${RESULT} ${TEMP})
endmacro()

macro(module_include_content MODULE_NAME MODULE_DIR PUBLIC_CONTENT_ACCESS_TYPE PRIVATE_CONTENT_ACCESS_TYPE)
	get_paths_by_pattern(PUBLIC_CONTENT ${MODULE_DIR} Public Private)
	get_paths_by_pattern(PRIVATE_CONTENT ${MODULE_DIR} Private Public)

	target_include_directories(${MODULE_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${PUBLIC_CONTENT})
	target_include_directories(${MODULE_NAME} ${PRIVATE_CONTENT_ACCESS_TYPE} ${PRIVATE_CONTENT})
endmacro()

macro(module_output_properties MODULE_NAME OUTPUT_NAME OUTPUT_DIRECTORY)
	set_target_properties(${MODULE_NAME} PROPERTIES
		RUNTIME_OUTPUT_NAME ${OUTPUT_NAME}
		ARCHIVE_OUTPUT_NAME ${OUTPUT_NAME}
		LIBRARY_OUTPUT_NAME ${OUTPUT_NAME}
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
		ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
		LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
endmacro()

macro(define_module MODULE_TYPE PUBLIC_CONTENT_ACCESS_TYPE PRIVATE_CONTENT_ACCESS_TYPE)
	file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/*.h)
	file(GLOB_RECURSE ${PROJECT_NAME}_INTERMEDIATE_SOURCES ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_INTERMEDIATE_HEADERS ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME}/*.h)

	add_library(${PROJECT_NAME} ${MODULE_TYPE}
		${${PROJECT_NAME}_SOURCES} 
		${${PROJECT_NAME}_HEADERS} 
		${${PROJECT_NAME}_INTERMEDIATE_SOURCES}
		${${PROJECT_NAME}_INTERMEDIATE_HEADERS})

	target_include_directories(${PROJECT_NAME} ${PRIVATE_CONTENT_ACCESS_TYPE} ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME})
	module_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR} ${PUBLIC_CONTENT_ACCESS_TYPE} ${PRIVATE_CONTENT_ACCESS_TYPE})
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
	module_output_properties(${PROJECT_NAME} ${PROJECT_NAME} ${BLOODSHOT_BINARIES_DIR})
	
	source_group(${PROJECT_NAME})
endmacro()

macro(define_shared_module)
	define_module(SHARED PUBLIC PRIVATE)
endmacro()

macro(define_interface_module)
	define_module(INTERFACE INTERFACE INTERFACE)
endmacro()

macro(define_program PROGRAM_NAME)
	file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

	add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})

	module_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR} PRIVATE PRIVATE)
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
	module_output_properties(${PROJECT_NAME} ${PROGRAM_NAME} ${BLOODSHOT_BINARIES_DIR})
	
	source_group(${PROJECT_NAME})
endmacro()

macro(define_user_module MODULE_TYPE PUBLIC_CONTENT_ACCESS_TYPE PRIVATE_CONTENT_ACCESS_TYPE)
	file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/*.h)
	file(GLOB_RECURSE ${PROJECT_NAME}_INTERMEDIATE_SOURCES ${${PROJECT_NAME}_INTERMEDIATE_DIR}/${PROJECT_NAME}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_INTERMEDIATE_HEADERS ${${PROJECT_NAME}_INTERMEDIATE_DIR}/${PROJECT_NAME}/*.h)

	add_library(${PROJECT_NAME} ${MODULE_TYPE}
		${${PROJECT_NAME}_SOURCES} 
		${${PROJECT_NAME}_HEADERS} 
		${${PROJECT_NAME}_INTERMEDIATE_SOURCES}
		${${PROJECT_NAME}_INTERMEDIATE_HEADERS})

	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -P ${BLOODSHOT_CMAKE_MODULE_DIR}/Timestamp.cmake ${${PROJECT_NAME}_BINARIES_DIR}
		COMMENT "Generate Timestamp")
	
	target_link_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_BINARIES_DIR})

	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_SHARED_DIR})
	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)
	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_THIRD_PARTY_DIR}/glad/include)
	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_THIRD_PARTY_DIR}/glm)
	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_THIRD_PARTY_DIR}/enet/include)
	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image)
	target_include_directories(${PROJECT_NAME} ${PUBLIC_CONTENT_ACCESS_TYPE} ${BLOODSHOT_THIRD_PARTY_DIR}/assimp)
	target_include_directories(${PROJECT_NAME} ${PRIVATE_CONTENT_ACCESS_TYPE} ${${PROJECT_NAME}_INTERMEDIATE_DIR}/${PROJECT_NAME})

	module_include_content(${PROJECT_NAME} ${BLOODSHOT_SOURCE_DIR}/Runtime ${PUBLIC_CONTENT_ACCESS_TYPE} ${PRIVATE_CONTENT_ACCESS_TYPE})
	module_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR} ${PUBLIC_CONTENT_ACCESS_TYPE} ${PRIVATE_CONTENT_ACCESS_TYPE})

	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${${PROJECT_NAME}_PROJECT_FOLDER_NAME})
	module_output_properties(${PROJECT_NAME} ${PROJECT_NAME} ${${PROJECT_NAME}_BINARIES_DIR})
	
	source_group(${PROJECT_NAME})
endmacro()

macro(define_user_shared_module)
	define_user_module(SHARED PUBLIC PRIVATE)
endmacro()

macro(define_user_interface_module)
	define_user_module(INTERFACE INTERFACE INTERFACE)
endmacro()
