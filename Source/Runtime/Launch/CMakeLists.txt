project(Launch)

file(GLOB_RECURSE LAUNCH_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE LAUNCH_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${LAUNCH_SOURCES} ${LAUNCH_HEADERS})
executable_include_content(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${BLOODSHOT_PROJECT_FOLDER_NAME})
target_output_properties(${PROJECT_NAME} "Bloodshot Engine" ${BLOODSHOT_BINARIES_DIR})

source_group(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC Assets)
target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC GameFramework)
target_link_libraries(${PROJECT_NAME} PUBLIC Input)
target_link_libraries(${PROJECT_NAME} PUBLIC EditorUI)
target_link_libraries(${PROJECT_NAME} PUBLIC Networking)
target_link_libraries(${PROJECT_NAME} PUBLIC Project)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
target_link_libraries(${PROJECT_NAME} PUBLIC Scene)
target_link_libraries(${PROJECT_NAME} PUBLIC Shared)
