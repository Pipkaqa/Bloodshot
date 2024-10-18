cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project("Bloodshot")

set(CMAKE_CXX_STANDARD "23")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CTEST_OUTPUT_ON_FAILURE "1")

include(CTest)
enable_testing()

#Macros
	macro(get_paths_by_pattern _RESULT _TARGET_DIR _PATTERN _ANTIPATTERN)
		set(_TEMP "")
		list(APPEND _DIRS_TO_OBSERVE "${CMAKE_SOURCE_DIR}/${_TARGET_DIR}")
		list(LENGTH _DIRS_TO_OBSERVE _REMAINED)
		while(_REMAINED)
			foreach(_DIR ${_DIRS_TO_OBSERVE})
				file(GLOB _CONTENT RELATIVE "${_DIR}" "${_DIR}/*")
				foreach(_PART ${_CONTENT})
					if(IS_DIRECTORY "${_DIR}/${_PART}")
						if(${_PART} STREQUAL ${_PATTERN})
							file(GLOB_RECURSE _IN_PATTERN RELATIVE "${_DIR}/${_PART}" "${_DIR}/${_PART}/*.h")
							list(LENGTH _IN_PATTERN _CONTAINS)
							if(_CONTAINS)
								list(APPEND _TEMP "${_DIR}/${_PART}")
							endif()
						elseif(NOT ${_PART} STREQUAL ${_ANTIPATTERN})
							list(APPEND _DIRS_TO_OBSERVE "${_DIR}/${_PART}")
						endif()
					endif()
				endforeach()
				list(REMOVE_ITEM _DIRS_TO_OBSERVE "${_DIR}")
			endforeach()
			list(LENGTH _DIRS_TO_OBSERVE _REMAINED)
		endwhile()
		set(${_RESULT} ${_TEMP})
	endmacro()

	macro(target_source_group _TARGET)
		set(SOURCE_GROUP_DELIMITER "/")
		set(_LAST_DIR "")
		set(_FILES "")
		get_target_property(_SOURCES ${_TARGET} SOURCES)
		foreach(_FILE ${_SOURCES})                                            
			file(RELATIVE_PATH _RELATIVE_FILE "${PROJECT_SOURCE_DIR}" ${_FILE}) 
			get_filename_component(_DIR "${_RELATIVE_FILE}" PATH)             
			if(NOT "${_DIR}" STREQUAL "${_LAST_DIR}")
				if(_FILES)
					source_group("${_LAST_DIR}" files ${_FILES})
				endif()
				set(_FILES "")
			endif()
			set(_FILES ${_FILES} ${_FILE})
			set(_LAST_DIR "${_DIR}")
		endforeach()
		if(_FILES)
			source_group("${_LAST_DIR}" files ${_FILES})
		endif()
	endmacro()

#Global
	#Common
		set(SOURCE_DIR "Source")
		set(THIRD_PARTY_DIR "ThirdParty")
		set(PROJECT_FOLDER_NAME	"Bloodshot Game Engine")
	#Engine
		set(ENGINE_PROJECT_NAME "Engine")
		set(ENGINE_SOURCE_DIR "${SOURCE_DIR}/Engine")
		file(GLOB_RECURSE ENGINE_SOURCES "${ENGINE_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE ENGINE_HEADERS "${ENGINE_SOURCE_DIR}/*.h")
	#Editor
		set(EDITOR_PROJECT_NAME "Editor")
		set(EDITOR_SOURCE_DIR "${SOURCE_DIR}/Editor")
		file(GLOB_RECURSE EDITOR_SOURCES "${EDITOR_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE EDITOR_HEADERS "${EDITOR_SOURCE_DIR}/*.h")
	#Tests
		set(TESTS_PROJECT_NAME "Tests")
		set(TESTS_SOURCE_DIR "${SOURCE_DIR}/Tests")
		file(GLOB_RECURSE TESTS_SOURCES "${TESTS_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE TESTS_HEADERS "${TESTS_SOURCE_DIR}/*.h")

#ThirdParty
	#glfw
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
		set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
		add_subdirectory("${THIRD_PARTY_DIR}/glfw")
	#glad
		add_subdirectory("${THIRD_PARTY_DIR}/glad")
		set_target_properties("glad" PROPERTIES FOLDER "glad")
	#glm
		add_subdirectory("${THIRD_PARTY_DIR}/glm")
		set_target_properties("glm" PROPERTIES FOLDER "glm")
	#enet
		add_subdirectory("${THIRD_PARTY_DIR}/enet")
		set_target_properties("enet" PROPERTIES FOLDER "enet")

#Linker
	#Subsystem
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
	#EntryPoint
		#Temp
			set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
		#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#Source
	#Engine
		add_library(${ENGINE_PROJECT_NAME} STATIC
			"${ENGINE_SOURCES}"
			"${ENGINE_HEADERS}")

		get_paths_by_pattern(ENGINE_PUBLIC_CONTENT "Source/Engine" "Public" "Private")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC ${ENGINE_PUBLIC_CONTENT})
		get_paths_by_pattern(ENGINE_PRIVATE_CONTENT "Source/Engine" "Private" "Public")
		target_include_directories(${ENGINE_PROJECT_NAME} PRIVATE ${ENGINE_PRIVATE_CONTENT})

		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/glfw/include")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/glad/include")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/enet/include")

		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "glfw")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "glad")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "glm")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "enet")

		set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
		set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES
			LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug"
			LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release"
			LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo"
			LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")

	#Editor
		add_executable(${EDITOR_PROJECT_NAME}
			"${EDITOR_SOURCES}"
			"${EDITOR_HEADERS}"
			"${XPROJECT_SOURCES}"
			"${XPROJECT_HEADERS}")

		target_link_libraries(${EDITOR_PROJECT_NAME} PRIVATE "${ENGINE_PROJECT_NAME}")

		set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
		set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES 
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug" 
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release" 
			RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo" 
			RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")

	#Tests
		foreach(_TEST_PATH ${TESTS_SOURCES})
			get_filename_component(_TEST_EXEC_NAME_WE "${_TEST_PATH}" NAME_WE)
		
			add_executable(${_TEST_EXEC_NAME_WE} ${_TEST_PATH} ${TESTS_HEADERS})
		
			target_link_libraries(${_TEST_EXEC_NAME_WE} PRIVATE "${ENGINE_PROJECT_NAME}")
		
			set_target_properties(${_TEST_EXEC_NAME_WE} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
			set_target_properties(${_TEST_EXEC_NAME_WE} PROPERTIES 
				RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug" 
				RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release" 
				RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo" 
				RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")
		
			add_test(NAME ${_TEST_EXEC_NAME_WE} COMMAND ${_TEST_EXEC_NAME_WE})

			add_dependencies(${EDITOR_PROJECT_NAME} ${_TEST_EXEC_NAME_WE})
		endforeach()
		
#Project
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EDITOR_PROJECT_NAME}")

	target_source_group(${ENGINE_PROJECT_NAME})
	target_source_group(${EDITOR_PROJECT_NAME})

	#add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD COMMAND ctest -C $<CONFIGURATION> --output-on-failure)

	#if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	#	add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${EDITOR_PROJECT_NAME}>/../Debug ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Debug)
	#	add_custom_command(TARGET ${RUNTIME_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${RUNTIME_PROJECT_NAME}>/../Debug ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Debug)
	#elseif(${CMAKE_BUILD_TYPE} STREQUAL Release)
	#	add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${EDITOR_PROJECT_NAME}>/../Release ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Release)
	#	add_custom_command(TARGET ${RUNTIME_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${RUNTIME_PROJECT_NAME}>/../Release ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Release)
	#endif()

#Compiler
	#Engine
		#Warnings
			target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "_CRT_SECURE_NO_WARNINGS")
		#Multithreading
			#target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_MULTITHREADING_ON")
		#Logging
			target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_LOGGING_ON")
		#Profiling
			target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_PROFILING_ON")
		#Networking
			target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_NETWORKING_ON")
