file(GLOB MODULE_DIRS *)
list(REMOVE_ITEM MODULE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

foreach(MODULE_DIR ${MODULE_DIRS})
	add_subdirectory(${MODULE_DIR})
endforeach()
