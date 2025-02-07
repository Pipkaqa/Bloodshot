project(EditorUI)

define_module()

target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
