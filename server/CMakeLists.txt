if (WIN32)
    # Set compiler before project() declaration
    set(CMAKE_C_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/clang/bin/clang.exe" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/clang/bin/clang++.exe" CACHE STRING "" FORCE)

    # You might also need to set the compiler launcher
    set(CMAKE_C_COMPILER_LAUNCHER "" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER_LAUNCHER "" CACHE STRING "" FORCE)
endif()

# Add this near the top to handle Linux shared libraries
if(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()

cmake_minimum_required(VERSION 3.21)
project(${GAME_TITLE} VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
 

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        add_compile_options(-g -gcodeview)
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fuse-ld=lld -Wl,/debug,/pdb:${PROJECT_NAME}.pdb")
    else()
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -gdwarf-4")
    endif()
    add_compile_definitions(DEBUG=1)
else()
    message(FATAL_ERROR "Build type must be Debug. Release builds are not supported.")
endif()

 # The rest is compiled as a static library at runtime.
add_executable(${PROJECT_NAME} 
                main.c
                game/src/str_util.c
                game/src/str_util.h
                game/src/allocator.c
                game/src/allocator.h
                )