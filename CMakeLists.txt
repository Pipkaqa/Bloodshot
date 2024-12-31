cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project("Bloodshot Engine")

set(CMAKE_CXX_STANDARD "23")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CTEST_OUTPUT_ON_FAILURE "1")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

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

#Common
	set(SOURCE_DIR "Source")
	set(THIRD_PARTY_DIR "ThirdParty")
	set(PROJECT_FOLDER_NAME	"Bloodshot Engine")

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

	#stb_image
		set(STB_IMAGE_HEADER "${CMAKE_SOURCE_DIR}/${THIRD_PARTY_DIR}/stb_image/stb_image.h")

	#assimp
		add_subdirectory("${THIRD_PARTY_DIR}/assimp")
		set_target_properties("assimp" PROPERTIES FOLDER "assimp")

	#imgui
		set(IMGUI_IMPL_OPENGL3_SOURCE "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp")
		set(IMGUI_IMPL_OPENGL3_HEADER "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.h")

		set(IMGUI_IMPL_GLFW_SOURCE "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp")
		set(IMGUI_IMPL_GLFW_HEADER "${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h")

		file(GLOB IMGUI_SOURCES "${THIRD_PARTY_DIR}/imgui/*.cpp")
		file(GLOB IMGUI_HEADERS "${THIRD_PARTY_DIR}/imgui/*.h")

		add_library("imgui" STATIC
			"${IMGUI_SOURCES}"
			"${IMGUI_HEADERS}"
			"${IMGUI_IMPL_OPENGL3_SOURCE}"
			"${IMGUI_IMPL_OPENGL3_HEADER}"
			"${IMGUI_IMPL_GLFW_SOURCE}"
			"${IMGUI_IMPL_GLFW_HEADER}")

		set_target_properties("imgui" PROPERTIES FOLDER "imgui")

		target_include_directories(imgui PRIVATE "${THIRD_PARTY_DIR}/imgui")
		target_include_directories(imgui PRIVATE "${THIRD_PARTY_DIR}/imgui/backends")
		target_include_directories(imgui PRIVATE "${THIRD_PARTY_DIR}/glfw/include")

#Linker
	#Subsystem
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")

	#EntryPoint
		#Temp
			set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
		#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#Source
	#Launcher
		set(LAUNCHER_PROJECT_NAME "Launcher")
		set(LAUNCHER_SOURCE_DIR "${SOURCE_DIR}/Launcher")
		file(GLOB_RECURSE LAUNCHER_SOURCES "${LAUNCHER_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE LAUNCHER_HEADERS "${LAUNCHER_SOURCE_DIR}/*.h")

		add_executable(${LAUNCHER_PROJECT_NAME}
			"${LAUNCHER_SOURCES}"
			"${LAUNCHER_HEADERS}"
			"${STB_IMAGE_HEADER}")

		get_paths_by_pattern(LAUNCHER_PUBLIC_CONTENT "Source/${LAUNCHER_PROJECT_NAME}" "Public" "Private")
		get_paths_by_pattern(LAUNCHER_PRIVATE_CONTENT "Source/${LAUNCHER_PROJECT_NAME}" "Private" "Public")

		target_include_directories(${LAUNCHER_PROJECT_NAME} PUBLIC ${LAUNCHER_PUBLIC_CONTENT})
		target_include_directories(${LAUNCHER_PROJECT_NAME} PRIVATE ${LAUNCHER_PRIVATE_CONTENT})

		target_include_directories(${LAUNCHER_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/glfw/include")
		target_include_directories(${LAUNCHER_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/glad/include")
		target_include_directories(${LAUNCHER_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/stb_image")
		target_include_directories(${LAUNCHER_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/imgui")
		target_include_directories(${LAUNCHER_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/imgui/backends")

		target_link_libraries(${LAUNCHER_PROJECT_NAME} PUBLIC "glfw")
		target_link_libraries(${LAUNCHER_PROJECT_NAME} PUBLIC "glad")
		target_link_libraries(${LAUNCHER_PROJECT_NAME} PUBLIC "imgui")
								
		set_target_properties(${LAUNCHER_PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
		set_target_properties(${LAUNCHER_PROJECT_NAME} PROPERTIES
			LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug"
			LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release"
			LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo"
			LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")
						
		target_source_group(${LAUNCHER_PROJECT_NAME})

	#Engine
		set(ENGINE_PROJECT_NAME "Engine")
		set(ENGINE_SOURCE_DIR "${SOURCE_DIR}/Engine")
		file(GLOB_RECURSE ENGINE_SOURCES "${ENGINE_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE ENGINE_HEADERS "${ENGINE_SOURCE_DIR}/*.h")

		add_library(${ENGINE_PROJECT_NAME} STATIC
			"${ENGINE_SOURCES}"
			"${ENGINE_HEADERS}"
			"${STB_IMAGE_HEADER}")

		get_paths_by_pattern(ENGINE_PUBLIC_CONTENT ${ENGINE_SOURCE_DIR} "Public" "Private")
		get_paths_by_pattern(ENGINE_PRIVATE_CONTENT ${ENGINE_SOURCE_DIR} "Private" "Public")

		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC ${ENGINE_PUBLIC_CONTENT})
		target_include_directories(${ENGINE_PROJECT_NAME} PRIVATE ${ENGINE_PRIVATE_CONTENT})

		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/glfw/include")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/glad/include")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/enet/include")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/stb_image")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/assimp")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/imgui")
		target_include_directories(${ENGINE_PROJECT_NAME} PUBLIC "${THIRD_PARTY_DIR}/imgui/backends")

		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "glfw")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "glad")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "glm")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "enet")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "assimp")
		target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC "imgui")

		set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
		set_target_properties(${ENGINE_PROJECT_NAME} PROPERTIES
			LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug"
			LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release"
			LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo"
			LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")

		target_source_group(${ENGINE_PROJECT_NAME})

		target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "_CRT_SECURE_NO_WARNINGS")
		target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_MULTITHREADING_ON")
		target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_LOGGING_ON")
		target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_PROFILING_ON")
		target_compile_definitions(${ENGINE_PROJECT_NAME} PUBLIC "BS_NETWORKING_ON")

	#Editor
		set(EDITOR_PROJECT_NAME "Editor")
		set(EDITOR_SOURCE_DIR "${SOURCE_DIR}/Editor")
		file(GLOB_RECURSE EDITOR_SOURCES "${EDITOR_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE EDITOR_HEADERS "${EDITOR_SOURCE_DIR}/*.h")

		add_executable(${EDITOR_PROJECT_NAME}
			"${EDITOR_SOURCES}"
			"${EDITOR_HEADERS}")

		get_paths_by_pattern(EDITOR_PUBLIC_CONTENT ${EDITOR_SOURCE_DIR} "Public" "Private")
		get_paths_by_pattern(EDITOR_PRIVATE_CONTENT ${EDITOR_SOURCE_DIR} "Private" "Public")

		target_include_directories(${EDITOR_PROJECT_NAME} PUBLIC ${EDITOR_PUBLIC_CONTENT})
		target_include_directories(${EDITOR_PROJECT_NAME} PRIVATE ${EDITOR_PRIVATE_CONTENT})

		target_link_libraries(${EDITOR_PROJECT_NAME} PRIVATE "${ENGINE_PROJECT_NAME}")

		set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
		set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES 
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug" 
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release" 
			RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo" 
			RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")

		target_source_group(${EDITOR_PROJECT_NAME})

	#HeaderTool
		set(HEADER_TOOL_PROJECT_NAME "HeaderTool")
		set(HEADER_TOOL_SOURCE_DIR "${SOURCE_DIR}/HeaderTool")
		file(GLOB_RECURSE HEADER_TOOL_SOURCES "${HEADER_TOOL_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE HEADER_TOOL_HEADERS "${HEADER_TOOL_SOURCE_DIR}/*.h")

		add_executable(${HEADER_TOOL_PROJECT_NAME}
			"${HEADER_TOOL_SOURCES}"
			"${HEADER_TOOL_HEADERS}")

		get_paths_by_pattern(HEADER_TOOL_PUBLIC_CONTENT ${HEADER_TOOL_SOURCE_DIR} "Public" "Private")
		get_paths_by_pattern(HEADER_TOOL_PRIVATE_CONTENT ${HEADER_TOOL_SOURCE_DIR} "Private" "Public")

		target_include_directories(${HEADER_TOOL_PROJECT_NAME} PUBLIC ${HEADER_TOOL_PUBLIC_CONTENT})
		target_include_directories(${HEADER_TOOL_PROJECT_NAME} PRIVATE ${HEADER_TOOL_PRIVATE_CONTENT})

		target_link_libraries(${HEADER_TOOL_PROJECT_NAME} PRIVATE "${ENGINE_PROJECT_NAME}")

		set_target_properties(${HEADER_TOOL_PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_FOLDER_NAME}")
		set_target_properties(${HEADER_TOOL_PROJECT_NAME} PROPERTIES 
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Binaries/Debug" 
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Binaries/Release" 
			RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/Binaries/RelWithDebInfo" 
			RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/Binaries/MinSizeRel")

		target_source_group(${HEADER_TOOL_PROJECT_NAME})

	#UnitTests
		set(TESTS_PROJECT_NAME "UnitTests")
		set(TESTS_SOURCE_DIR "${SOURCE_DIR}/UnitTests")
		file(GLOB_RECURSE TESTS_SOURCES "${TESTS_SOURCE_DIR}/*.cpp")
		file(GLOB_RECURSE TESTS_HEADERS "${TESTS_SOURCE_DIR}/*.h")

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

	#add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD COMMAND ctest -C $<CONFIGURATION> --output-on-failure)

	#if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	#	add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${EDITOR_PROJECT_NAME}>/../Debug ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Debug)
	#	add_custom_command(TARGET ${RUNTIME_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${RUNTIME_PROJECT_NAME}>/../Debug ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Debug)
	#elseif(${CMAKE_BUILD_TYPE} STREQUAL Release)
	#	add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${EDITOR_PROJECT_NAME}>/../Release ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Release)
	#	add_custom_command(TARGET ${RUNTIME_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${RUNTIME_PROJECT_NAME}>/../Release ${CMAKE_CURRENT_SOURCE_DIR}/Launch/Release)
	#endif()
