project(HeaderTool)

file(GLOB_RECURSE HEADERTOOL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERTOOL_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${HEADERTOOL_SOURCES} ${HEADERTOOL_HEADERS})
executable_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_SHARED_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Shared)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
target_output_properties(${PROJECT_NAME} "Bloodshot Header Tool" ${BLOODSHOT_BINARIES_DIR})

source_group(${PROJECT_NAME})
