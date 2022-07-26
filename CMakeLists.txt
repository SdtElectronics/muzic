# Copyright 2022 SdtElectronics
# SPDX-License-Identifier: Zlib

cmake_minimum_required(VERSION 3.19)
project(muzic VERSION 1.0.0)

option(BUILD_SHARED "Build shared library" ON)
option(BUILD_TESTS "Build test" ON)
option(BUILD_EXAMPLES "Build examples" ON)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

add_library(muzic
    defl_static.c
    genlz77.c
    inflater.c
    zlib_comp.c
)

if (BUILD_SHARED)
    add_library(muzic_s SHARED
        defl_static.c
        genlz77.c
        inflater.c
        zlib_comp.c
    )

    set_target_properties(muzic_s PROPERTIES OUTPUT_NAME muzic)
endif()

if (DEBUG_MSG)
    target_compile_definitions(muzic PRIVATE
        MZ_DEBUG
    )

    target_compile_definitions(muzic_s PRIVATE
        MZ_DEBUG
    )   
endif()

target_include_directories(muzic 
  PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>")

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
