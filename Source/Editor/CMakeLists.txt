project(Editor)

file(GLOB_RECURSE EDITOR_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE EDITOR_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${EDITOR_SOURCES} ${EDITOR_HEADERS})
executable_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
target_output_properties(${PROJECT_NAME} ${PROJECT_NAME} ${BLOODSHOT_BINARIES_DIR})

source_group(${PROJECT_NAME})
