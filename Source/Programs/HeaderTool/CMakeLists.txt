project(HeaderTool)

file(GLOB_RECURSE HEADERTOOL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERTOOL_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${HEADERTOOL_SOURCES} ${HEADERTOOL_HEADERS})
module_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR} PRIVATE PRIVATE)

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_SHARED_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Shared)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
module_output_properties(${PROJECT_NAME} "Bloodshot Header Tool" ${BLOODSHOT_BINARIES_DIR})

source_group(${PROJECT_NAME})
