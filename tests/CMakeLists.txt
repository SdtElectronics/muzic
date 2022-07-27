# Copyright 2022 SdtElectronics
# SPDX-License-Identifier: Zlib

########################### Integration test executables ############################

add_executable(test_main ${PROJECT_SOURCE_DIR}/tests/integration.c)

target_link_libraries(test_main PRIVATE muzic)

####################### Interop test executables using muzic  #######################

add_executable(muzicDefl ${PROJECT_SOURCE_DIR}/examples/defl2stdout.c)

target_link_libraries(muzicDefl PRIVATE muzic)


add_executable(muzicDeflFragIn ${PROJECT_SOURCE_DIR}/examples/defl2stdout.c)

target_compile_definitions(muzicDeflFragIn PRIVATE MZ_EG_INPUT_BUFFER_SIZE=4)

target_link_libraries(muzicDeflFragIn PRIVATE muzic)


add_executable(muzicDeflFragOut ${PROJECT_SOURCE_DIR}/examples/defl2stdout.c)

target_compile_definitions(muzicDeflFragOut PRIVATE MZ_EG_OUTPUT_BUFFER_SIZE=4)

target_link_libraries(muzicDeflFragOut PRIVATE muzic)


add_executable(muzicInfl ${PROJECT_SOURCE_DIR}/examples/infl4stdin.c)

target_link_libraries(muzicInfl PRIVATE muzic)


add_executable(muzicInflFragIn ${PROJECT_SOURCE_DIR}/examples/infl4stdin.c)

target_compile_definitions(muzicInflFragIn PRIVATE MZ_EG_INPUT_BUFFER_SIZE=3)

target_link_libraries(muzicInflFragIn PRIVATE muzic)


add_executable(muzicInflFragOut ${PROJECT_SOURCE_DIR}/examples/infl4stdin.c)

target_compile_definitions(muzicInflFragOut PRIVATE MZ_EG_OUTPUT_BUFFER_SIZE=4)

target_link_libraries(muzicInflFragOut PRIVATE muzic)

######################## Interop test executables using Zlib ########################

find_package(ZLIB)

add_executable(zlibDefl ${PROJECT_SOURCE_DIR}/examples/defl2stdout.c)

target_compile_definitions(zlibDefl PRIVATE
    MZ_EG_INPUT_BUFFER_SIZE=4096
    MZ_EG_USE_ZLIB
)

target_link_libraries(zlibDefl PRIVATE ZLIB::ZLIB)


add_executable(zlibDeflFragIn ${PROJECT_SOURCE_DIR}/examples/defl2stdout.c)

target_compile_definitions(zlibDeflFragIn PRIVATE
    MZ_EG_INPUT_BUFFER_SIZE=4
    MZ_EG_USE_ZLIB
)

target_link_libraries(zlibDeflFragIn PRIVATE ZLIB::ZLIB)


add_executable(zlibDeflFragOut ${PROJECT_SOURCE_DIR}/examples/defl2stdout.c)

target_compile_definitions(zlibDeflFragOut PRIVATE
    MZ_EG_INPUT_BUFFER_SIZE=4096
    MZ_EG_OUTPUT_BUFFER_SIZE=16
    MZ_EG_USE_ZLIB
)

target_link_libraries(zlibDeflFragOut PRIVATE ZLIB::ZLIB)


add_executable(zlibInfl ${PROJECT_SOURCE_DIR}/examples/infl4stdin.c)

target_compile_definitions(zlibInfl PRIVATE MZ_EG_USE_ZLIB)

target_link_libraries(zlibInfl PRIVATE ZLIB::ZLIB)


add_executable(zlibInflFragIn ${PROJECT_SOURCE_DIR}/examples/infl4stdin.c)

target_compile_definitions(zlibInflFragIn PRIVATE
    MZ_EG_INPUT_BUFFER_SIZE=3
    MZ_EG_USE_ZLIB
)

target_link_libraries(zlibInflFragIn PRIVATE ZLIB::ZLIB)
