project(Networking)

define_shared_module()

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/enet/include)

target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC enet)

if(BS_NETWORKING)
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_NETWORKING_ON)
endif()
