project(EditorUI)

define_interface_module()

target_link_libraries(${PROJECT_NAME} INTERFACE Core)
target_link_libraries(${PROJECT_NAME} INTERFACE Rendering)
target_link_libraries(${PROJECT_NAME} INTERFACE imgui)
