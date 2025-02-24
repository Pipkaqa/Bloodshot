project(Assets)

define_shared_module()

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/assimp)

target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
