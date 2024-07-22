cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(glad)

add_library(${PROJECT_NAME} STATIC "include/glad/glad.h" "src/glad.c")
target_include_directories(${PROJECT_NAME} PUBLIC "include")
