if (WIN32)
    # Set compiler before project() declaration
    set(CMAKE_C_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/../clang/bin/clang.exe" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/../clang/bin/clang++.exe" CACHE STRING "" FORCE)

    # You might also need to set the compiler launcher
    set(CMAKE_C_COMPILER_LAUNCHER "" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER_LAUNCHER "" CACHE STRING "" FORCE)
endif()

cmake_minimum_required(VERSION 3.21)
project(${GAME_TITLE} VERSION 1.0 LANGUAGES C) # ${GAME_TITLE} is defined in Makefile
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenGL
if (WIN32)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    include_directories( ${OPENGL_INCLUDE_DIRS} )
else()
    find_package(OpenGL REQUIRED)
endif()

file(GLOB_RECURSE SOURCES
    src/*.c
    src/*.h
)

# Enable debug symbols and set debug flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        add_compile_options(-g -gcodeview)
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fuse-ld=lld -Wl,/debug,/pdb:${PROJECT_NAME}.pdb")
    else()
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -gdwarf-4")
    endif()
    add_compile_definitions(DEBUG=1)

    if (RUN_TESTS)
        add_executable(${PROJECT_NAME} ${SOURCES})
    else()
        add_library(${PROJECT_NAME} SHARED ${SOURCES})
    endif()
else()
    if(WIN32)
        add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
    else()
        add_executable(${PROJECT_NAME} ${SOURCES})
    endif()
endif()


# Include directories (for GLFW and OpenGL)
# (Optional if GLFW3_INCLUDE_DIRS and OPENGL_INCLUDE_DIRS are set by the find_package)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

if(WIN32)
    # message(STATUS "Configuring for Windows")
    add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glfw_win)
    include_directories(${CMAKE_SOURCE_DIR}/libs/glfw_win/include)
    add_compile_definitions(_CRT_ALIGNED_MALLOC=1)
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
    $<$<NOT:$<PLATFORM_ID:Windows>>:m>
)

# For the executable target, explicitly set include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

if(WIN32 AND NOT RUN_TESTS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MODULE_EXPORTS)
endif()

