project(Launcher)

define_program("Bloodshot Engine Launcher")

target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_SHARED_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends)

target_link_libraries(${PROJECT_NAME} PRIVATE Shared)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
