cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

option(BS_MULTITHREADING "Multithreading in engine" ON)
option(BS_LOGGING "Logging in engine" OFF)
option(BS_PROFILING "Profiling in engine" OFF)
option(BS_NETWORKING "Networking in engine" ON)

project("Bloodshot Engine")

set(CMAKE_CXX_STANDARD 23)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CTEST_OUTPUT_ON_FAILURE 1)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Common
set(PROJECT_FOLDER_NAME	${PROJECT_NAME})
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)
set(SHARED_DIR ${SOURCE_DIR}/Shared)
set(PROGRAMS_DIR ${SOURCE_DIR}/Programs)
set(INTERMEDIATE_DIR ${CMAKE_SOURCE_DIR}/Intermediate)
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/Binaries)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Shared/cmake)
set(BINARY_DIR ${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_BUILD_TYPE})

#Modules
include(Macros)
include(CTest)
enable_testing()

#glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${THIRD_PARTY_DIR}/glfw)

#glad
add_subdirectory(${THIRD_PARTY_DIR}/glad)
set_target_properties(glad PROPERTIES FOLDER glad)

#glm
add_subdirectory(${THIRD_PARTY_DIR}/glm)
set_target_properties(glm PROPERTIES FOLDER glm)

#enet
add_subdirectory(${THIRD_PARTY_DIR}/enet)
set_target_properties(enet PROPERTIES FOLDER enet)

#stb_image
set(STB_IMAGE_HEADER ${THIRD_PARTY_DIR}/stb_image/stb_image.h)

#assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${THIRD_PARTY_DIR}/assimp)
set_target_properties(assimp PROPERTIES FOLDER assimp)

#imgui
set(IMGUI_IMPL_OPENGL3_SOURCE ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
set(IMGUI_IMPL_OPENGL3_HEADER ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.h)

set(IMGUI_IMPL_GLFW_SOURCE ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp)
set(IMGUI_IMPL_GLFW_HEADER ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h)

file(GLOB IMGUI_SOURCES ${THIRD_PARTY_DIR}/imgui/*.cpp)
file(GLOB IMGUI_HEADERS ${THIRD_PARTY_DIR}/imgui/*.h)

add_library(imgui STATIC
	${IMGUI_SOURCES}
	${IMGUI_HEADERS}
	${IMGUI_IMPL_OPENGL3_SOURCE}
	${IMGUI_IMPL_OPENGL3_HEADER}
	${IMGUI_IMPL_GLFW_SOURCE}
	${IMGUI_IMPL_GLFW_HEADER})

set_target_properties(imgui PROPERTIES FOLDER imgui)

target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/imgui)
target_include_directories(imgui PUBLIC ${THIRD_PARTY_DIR}/imgui/backends)
target_include_directories(imgui PRIVATE ${THIRD_PARTY_DIR}/glfw/include)

#Shared
add_subdirectory(${SHARED_DIR})

#Engine
add_subdirectory(${SOURCE_DIR}/Engine)

#Editor
add_subdirectory(${SOURCE_DIR}/Editor)

#Programs
add_subdirectory(${PROGRAMS_DIR})

#Tests
add_subdirectory(${SOURCE_DIR}/Tests)

#Other
set(CMAKE_EXE_LINKER_FLAGS_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup)
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EDITOR_PROJECT_NAME})
