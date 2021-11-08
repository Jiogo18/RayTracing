cmake_minimum_required(VERSION 3.10)

# Project
project(RayTracing)

# Sources
file(GLOB_RECURSE SOURCES src/**.cpp)
file(GLOB_RECURSE HEADERS src/**.h)
file(GLOB_RECURSE RESOURCES src/RayTracing.rc)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# C++ 20
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
