# Copyright 2022 SdtElectronics
# SPDX-License-Identifier: Zlib

set(DEFL_INPUT_BUFFER_SIZE "256" CACHE STRING "input buffer size for defl2stdout" FORCE)
set(DEFL_OUTPUT_BUFFER_SIZE "1024" CACHE STRING "output buffer size for defl2stdout" FORCE)

set(INFL_INPUT_BUFFER_SIZE "192" CACHE STRING "input buffer size for infl4stdin" FORCE)
set(INFL_OUTPUT_BUFFER_SIZE "768" CACHE STRING "output buffer size for infl4stdin" FORCE)


add_executable(defl2stdout defl2stdout.c)

target_compile_definitions(defl2stdout PRIVATE
    MZ_EG_INPUT_BUFFER_SIZE=${DEFL_INPUT_BUFFER_SIZE}
    MZ_EG_OUTPUT_BUFFER_SIZE=${DEFL_OUTPUT_BUFFER_SIZE}
)

if(DEFL_USE_ZLIB)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    find_package(ZLIB)

    target_compile_definitions(defl2stdout PRIVATE
        MZ_EG_USE_ZLIB
    )

    target_link_libraries(defl2stdout PRIVATE ZLIB::ZLIB)
else()
    target_link_libraries(defl2stdout PRIVATE muzic)
endif()

add_executable(infl4stdin infl4stdin.c)

target_compile_definitions(infl4stdin PRIVATE
    MZ_EG_INPUT_BUFFER_SIZE=${INFL_INPUT_BUFFER_SIZE}
    MZ_EG_OUTPUT_BUFFER_SIZE=${INFL_OUTPUT_BUFFER_SIZE}
)

if(INFL_USE_ZLIB)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    find_package(ZLIB)

    target_compile_definitions(infl4stdin PRIVATE
        MZ_EG_USE_ZLIB
    )

    target_link_libraries(infl4stdin PRIVATE ZLIB::ZLIB)
else()
    target_link_libraries(infl4stdin PRIVATE muzic)
endif()
