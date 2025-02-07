project(Launcher)

file(GLOB_RECURSE LAUNCHER_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE LAUNCHER_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${LAUNCHER_SOURCES} ${LAUNCHER_HEADERS} ${STB_IMAGE_HEADER})
executable_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_SHARED_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends)

target_link_libraries(${PROJECT_NAME} PUBLIC Shared)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
				
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
target_output_properties(${PROJECT_NAME} "Bloodshot Engine Launcher" ${BLOODSHOT_BINARIES_DIR})

source_group(${PROJECT_NAME})
