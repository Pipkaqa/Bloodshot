file(GLOB PROGRAM_DIRS *)
list(REMOVE_ITEM PROGRAM_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

foreach(PROGRAM_DIR ${PROGRAM_DIRS})
	add_subdirectory(${PROGRAM_DIR})
endforeach()
