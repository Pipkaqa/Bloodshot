project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_custom_command(OUTPUT ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME}/Engine.gen.cpp
	COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}
	${BLOODSHOT_BINARIES_DIR}/HeaderTool.exe Source:${BLOODSHOT_SOURCE_DIR}/Engine 
	Output:${BLOODSHOT_INTERMEDIATE_DIR}/Engine
	Module:Engine
	OutputToSingleFiles:Yes
	COMMENT "Run HeaderTool on Engine sources")

add_library(${PROJECT_NAME} STATIC
	${ENGINE_SOURCES}
	${ENGINE_HEADERS}
	${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME}/Engine.gen.cpp
	${STB_IMAGE_HEADER})
	
library_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_SHARED_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_INTERMEDIATE_DIR}/${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/enet/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/assimp)

target_link_libraries(${PROJECT_NAME} PUBLIC Shared)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC enet)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
target_output_properties(${PROJECT_NAME} ${PROJECT_NAME} ${BLOODSHOT_BINARIES_DIR})

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
