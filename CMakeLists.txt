if (WIN32)
    # Set compiler before project() declaration
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/clang.exe" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/clang++.exe" CACHE STRING "" FORCE)

    # You might also need to set the compiler launcher
    set(CMAKE_C_COMPILER_LAUNCHER "" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER_LAUNCHER "" CACHE STRING "" FORCE)
endif()

cmake_minimum_required(VERSION 3.21)

# Project name and C standard
project(${GAME_TITLE} VERSION 1.0 LANGUAGES C)

# Enable C23 standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenGL
if (WIN32)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    include_directories( ${OPENGL_INCLUDE_DIRS} )
else()
    find_package(OpenGL REQUIRED)
endif()

# Automatically find all source and header files in the src directory
file(GLOB_RECURSE SOURCES
    src/*.c
    src/*.h
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories (for GLFW and OpenGL)
# (Optional if GLFW3_INCLUDE_DIRS and OPENGL_INCLUDE_DIRS are set by the find_package)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# OS-specific configuration
if(WIN32)
    # message(STATUS "Configuring for Windows")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glfw_win)
    include_directories(${CMAKE_SOURCE_DIR}/libs/glfw_win/include)
else()
    message(STATUS "Configuring for Linux or other platforms")
    include_directories(${CMAKE_SOURCE_DIR}/libs/glfw_linux/include)
endif()

if (RUN_TESTS)
    add_compile_definitions(TEST_MODE=1)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    m
)

