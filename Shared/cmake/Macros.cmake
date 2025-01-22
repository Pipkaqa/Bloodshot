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

macro(library_include_content TARGET_NAME)
	get_paths_by_pattern(PUBLIC_CONTENT ${PROJECT_SOURCE_DIR} Public Private)
	get_paths_by_pattern(PRIVATE_CONTENT ${PROJECT_SOURCE_DIR} Private Public)

	target_include_directories(${TARGET_NAME} PUBLIC ${PUBLIC_CONTENT})
	target_include_directories(${TARGET_NAME} PRIVATE ${PRIVATE_CONTENT})
endmacro()

macro(executable_include_content TARGET_NAME)
	get_paths_by_pattern(PUBLIC_CONTENT ${PROJECT_SOURCE_DIR} Public Private)
	get_paths_by_pattern(PRIVATE_CONTENT ${PROJECT_SOURCE_DIR} Private Public)

	target_include_directories(${TARGET_NAME} PRIVATE ${PUBLIC_CONTENT})
	target_include_directories(${TARGET_NAME} PRIVATE ${PRIVATE_CONTENT})
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