project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE ENGINE_INTERMEDIATE_HEADERS ${INTERMEDIATE_DIR}/${PROJECT_NAME}/*.h)

add_library(${PROJECT_NAME} STATIC
	${ENGINE_SOURCES}
	${ENGINE_HEADERS}
	${ENGINE_INTERMEDIATE_HEADERS}
	${STB_IMAGE_HEADER})
	
library_include_content(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${SHARED_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${INTERMEDIATE_DIR}/${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/enet/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/stb_image)
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/assimp)

target_link_libraries(${PROJECT_NAME} PUBLIC Shared)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC enet)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_FOLDER_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/Debug
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/Release
	LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}/RelWithDebInfo
	LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${OUTPUT_DIR}/MinSizeRel)

source_group(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

#these options not working (cuz of subdirectory), fix it
if(BS_MULTITHREADING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_MULTITHREADING_ON)
endif()

if(BS_LOGGING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_LOGGING_ON)
endif()

if(BS_PROFILING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_PROFILING_ON)
endif()

if(BS_NETWORKING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_NETWORKING_ON)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND}
	-E copy $<TARGET_FILE:${PROJECT_NAME}> ${OUTPUT_DIR}/${CMAKE_BUILD_TYPE})

