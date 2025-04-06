project(Launch)

define_program("Bloodshot Engine")

target_link_libraries(${PROJECT_NAME} PUBLIC Assets)
target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC GameFramework)
target_link_libraries(${PROJECT_NAME} PUBLIC Input)
target_link_libraries(${PROJECT_NAME} PUBLIC EditorUI)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
target_link_libraries(${PROJECT_NAME} PUBLIC Scene)
target_link_libraries(${PROJECT_NAME} PUBLIC Shared)
