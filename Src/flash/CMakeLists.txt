cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C)

set(Flash_Src
    ${CMAKE_SOURCE_DIR}/src/flash/flash.c
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${Flash_Src})