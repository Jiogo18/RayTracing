cmake_minimum_required(VERSION 3.10)

if(MSVC)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
endif()

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

## Include libraries
set(LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libraries)
# OpenCV (https://github.com/opencv/opencv/tree/4.x)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# OpenGL & GLUT
set(GLUT_ROOT_PATH ${LIBRARIES_DIR}/freeglut)
set(OPENGL_LIBRARY_DIR ${GLUT_ROOT_PATH}/build/lib) # Build freeglut in ./libraries/freeglut/build

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
if(NOT OPENGL_FOUND)
    message(ERROR "OPENGL not found!")
endif(NOT OPENGL_FOUND)

# GLUT (submodule freeglut)
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
if(NOT GLUT_FOUND)
    message(ERROR "GLUT not found!")
endif(NOT GLUT_FOUND)
# copy libfreeglut.dll to build folder
if(EXISTS ${GLUT_ROOT_PATH}/build/bin/libfreeglut.dll)
    file(COPY ${GLUT_ROOT_PATH}/build/bin/libfreeglut.dll DESTINATION ${CMAKE_BINARY_DIR})
endif()

# OpenCL (submodule OpenCL-SDK)
set(OpenCL_INCLUDE_DIR ${LIBRARIES_DIR}/OpenCL-SDK/external/OpenCL-CLHPP/include)
set(OPENCL_CLHPP_HEADERS_DIR ${LIBRARIES_DIR}/OpenCL-SDK/external/OpenCL-Headers)
if(EXISTS ${LIBRARIES_DIR}/OpenCL-SDK/build/external/OpenCL-ICD-Loader/Release/OpenCL.dll)
    file(COPY ${LIBRARIES_DIR}/OpenCL-SDK/build/external/OpenCL-ICD-Loader/Release/OpenCL.dll DESTINATION ${CMAKE_BINARY_DIR})
    set(OpenCL_LIBRARY OpenCL.dll)
endif()
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
include_directories(${OPENCL_CLHPP_HEADERS_DIR})

# Linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLUT_LIBRARIES} OpenCL::OpenCL)
