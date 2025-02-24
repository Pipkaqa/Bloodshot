project(Rendering)

define_shared_module()

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image)

target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
