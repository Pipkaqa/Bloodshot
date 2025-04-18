project(Core)

define_shared_module()

target_include_directories(${PROJECT_NAME} PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)

target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if(${BS_LOGGING})
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_LOGGING_ON)
endif()

if(${BS_PROFILING})
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_PROFILING_ON)
endif()

if(${BS_MULTITHREADING})
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_MULTITHREADING_ON)
endif()

if(${BS_WITH_EDITOR})
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_WITH_EDITOR)
endif()

if(${BS_WITH_AUTOMATION_TESTS})
	target_compile_definitions(${PROJECT_NAME} PUBLIC BS_WITH_AUTOMATION_TESTS)
endif()
