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

macro(library_include_content TARGET_NAME TARGET_DIR)
	get_paths_by_pattern(PUBLIC_CONTENT ${TARGET_DIR} Public Private)
	get_paths_by_pattern(PRIVATE_CONTENT ${TARGET_DIR} Private Public)

	target_include_directories(${TARGET_NAME} PUBLIC ${PUBLIC_CONTENT})
	target_include_directories(${TARGET_NAME} PRIVATE ${PRIVATE_CONTENT})
endmacro()

macro(executable_include_content TARGET_NAME TARGET_DIR)
	get_paths_by_pattern(PUBLIC_CONTENT ${TARGET_DIR} Public Private)
	get_paths_by_pattern(PRIVATE_CONTENT ${TARGET_DIR} Private Public)

	target_include_directories(${TARGET_NAME} PRIVATE ${PUBLIC_CONTENT})
	target_include_directories(${TARGET_NAME} PRIVATE ${PRIVATE_CONTENT})
endmacro()

macro(target_output_properties TARGET_NAME OUTPUT_NAME OUTPUT_DIRECTORY)
	set_target_properties(${TARGET_NAME} PROPERTIES
		RUNTIME_OUTPUT_NAME ${OUTPUT_NAME}
		ARCHIVE_OUTPUT_NAME ${OUTPUT_NAME}
		LIBRARY_OUTPUT_NAME ${OUTPUT_NAME}
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
		ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
		LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
endmacro()

macro(define_module)
	file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/*.h)
	file(GLOB_RECURSE ${PROJECT_NAME}_INTERMEDIATE_SOURCES ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME}/*.cpp)
	file(GLOB_RECURSE ${PROJECT_NAME}_INTERMEDIATE_HEADERS ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME}/*.h)

	add_library(${PROJECT_NAME} SHARED 
		${${PROJECT_NAME}_SOURCES} 
		${${PROJECT_NAME}_HEADERS} 
		${${PROJECT_NAME}_INTERMEDIATE_SOURCES}
		${${PROJECT_NAME}_INTERMEDIATE_HEADERS})

	target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME})
	library_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
	target_output_properties(${PROJECT_NAME} ${PROJECT_NAME} ${BLOODSHOT_BINARIES_DIR})
	
	source_group(${PROJECT_NAME})
endmacro()

#macro(target_source_group TARGET)
#	set(SOURCE_GROUP_DELIMITER /)
#	set(LAST_DIR)
#	set(FILES)
#	get_target_property(SOURCES ${TARGET} SOURCES)
#	foreach(FILE ${SOURCES})                                            
#		file(RELATIVE_PATH RELATIVE_FILE ${PROJECT_SOURCE_DIR} ${FILE}) 
#		get_filename_component(_DIR ${RELATIVE_FILE} PATH)             
#		if(NOT ${_DIR} STREQUAL ${LAST_DIR})
#			if(FILES)
#				source_group(${LAST_DIR} files ${FILES})
#			endif()
#			set(FILES)
#		endif()
#		set(FILES ${FILES} ${FILE})
#		set(LAST_DIR ${_DIR})
#	endforeach()
#	if(FILES)
#		source_group(${LAST_DIR} files ${FILES})
#	endif()
#endmacro()