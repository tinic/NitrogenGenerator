cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 23)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME NitrogenGenerator)

# init sub modules
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.gitmodules")
  execute_process (COMMAND git submodule update --init --recursive
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif() 

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

add_subdirectory(magic_enum EXCLUDE_FROM_ALL)
target_link_libraries(${CMAKE_PROJECT_NAME} magic_enum::magic_enum)

add_subdirectory(fixed-containers EXCLUDE_FROM_ALL)
target_link_libraries(${CMAKE_PROJECT_NAME} fixed_containers::fixed_containers)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    screen.cpp
    callbacks.cpp
    mcp.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${PROJECT_BINARY_DIR}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    # Add user defined libraries
)

IF (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(FONTBM_BINARY ${PROJECT_SOURCE_DIR}/fontbm/build/Debug/fontbm.exe)
    set(FONTBM_CMAKE_COMMAND cmake --preset=default .. )
ELSE (${CMAKE_HOST_SYSTEM_NAME} STREQUAL  "Windows")
    set(FONTBM_BINARY ${PROJECT_SOURCE_DIR}/fontbm/build/fontbm)
    set(FONTBM_CMAKE_COMMAND cmake -Wno-deprecated -G "Ninja" .. )
ENDIF (${CMAKE_HOST_SYSTEM_NAME} STREQUAL  "Windows")

file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/fontbm/build)
add_custom_command(
    OUTPUT ${FONTBM_BINARY}
    COMMAND ${FONTBM_CMAKE_COMMAND}
    COMMAND cmake --build .
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/fontbm/build
    DEPENDS ${PROJECT_SOURCE_DIR}/fontbm/src/main.cpp
    USES_TERMINAL)

set(FONT_FNT ${PROJECT_BINARY_DIR}/font.fnt)
set(FONT_PNG ${PROJECT_BINARY_DIR}/font_0.png)
add_custom_command(
    OUTPUT ${FONT_PNG} ${FONT_FNT}
    COMMAND ${FONTBM_BINARY} --font-file ${PROJECT_SOURCE_DIR}/SF-Compact-Display-Bold.otf --texture-crop-width --texture-crop-height --kerning-pairs regular --tabular-numbers --slashed-zero --monochrome --font-size 18 --output font --data-format json
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS ${FONTBM_BINARY}
    USES_TERMINAL)

set(FONT_BITMAP_HEADER ${PROJECT_BINARY_DIR}/font_0.h)
add_custom_command(
        OUTPUT ${FONT_BITMAP_HEADER}
        COMMAND python3 ${PROJECT_SOURCE_DIR}/png_to_header.py -i ${FONT_PNG} -o ${FONT_BITMAP_HEADER} -n font_bitmap
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${FONT_PNG}
        USES_TERMINAL)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${FONT_BITMAP_HEADER})

set(FONT_HEADER ${PROJECT_BINARY_DIR}/font.h)
add_custom_command(
        OUTPUT ${FONT_HEADER}
        COMMAND python3 ${PROJECT_SOURCE_DIR}/font_to_header.py -i ${FONT_FNT} -o ${FONT_HEADER}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${FONT_FNT}
        USES_TERMINAL)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${FONT_HEADER})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${CMAKE_PROJECT_NAME}> > ${CMAKE_PROJECT_NAME}.S
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf
    USES_TERMINAL)
