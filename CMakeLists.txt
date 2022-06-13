cmake_minimum_required(VERSION 3.19)
project(sdefl VERSION 1.0.0)

option(BUILD_TESTS "Build test" ON)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

add_library(sdefl
    sdefl.c
    sinfl.c)

target_include_directories(sdefl 
  PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>")

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()