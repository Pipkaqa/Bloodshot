project(Input)

define_module()

target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
