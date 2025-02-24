project(Project)

define_interface_module()

target_link_libraries(${PROJECT_NAME} INTERFACE Core)
