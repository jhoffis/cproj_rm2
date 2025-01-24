cmake_minimum_required(VERSION 3.21)

# Project name and C standard
project(OpenGLApp VERSION 1.0 LANGUAGES C)

# Enable C23 standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Automatically find all source and header files in the src directory
file(GLOB_RECURSE SOURCES
    src/*.c
    src/*.h
)

# Add the executable
add_executable(OpenGLApp ${SOURCES})

# Include directories (for GLFW and OpenGL)
# (Optional if GLFW3_INCLUDE_DIRS and OPENGL_INCLUDE_DIRS are set by the find_package)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# OS-specific configuration
if(WIN32)
    message(STATUS "Configuring for Windows")
    include_directories(${CMAKE_SOURCE_DIR}/libs/glfw_win/include)
else()
    message(STATUS "Configuring for Linux or other platforms")
    include_directories(${CMAKE_SOURCE_DIR}/libs/glfw_linux/include)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw_linux/include)

# Link libraries
target_link_libraries(OpenGLApp PRIVATE
    OpenGL::GL
    glfw
)

