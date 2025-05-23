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
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.gitmodules")
    execute_process(COMMAND git submodule update --init --recursive
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
add_subdirectory(constixel)


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
    constixel::constixel
    # Add user defined libraries
)

# Generate version.h
find_package(Git)

execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_SHORT_SHA OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_REV_COUNT OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=%ad HEAD --date=iso-strict WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=%as HEAD --date=iso-strict WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_DATE_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file("${PROJECT_SOURCE_DIR}/version.h.in" "${CMAKE_BINARY_DIR}/version.h" @ONLY)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.h)

# Generate custom font with SF-Compact characters (always regenerate)
set(FONT_HEADER ${CMAKE_BINARY_DIR}/ibmplexsans_regular_18_mono.hpp)
set(PRIMARY_FONT ${PROJECT_SOURCE_DIR}/constixel/genfonts/otf/IBMPlexSans-Regular.otf)
set(SECONDARY_FONT ${PROJECT_SOURCE_DIR}/otf/SF-Compact-Display-Regular.otf)

# Create CMake script for font generation
set(FONT_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_font.cmake")

# Always regenerate the font by using a custom target instead of OUTPUT
add_custom_target(generate_font ALL
    COMMAND ${CMAKE_COMMAND} 
        -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
        -DCMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
        -P "${FONT_SCRIPT}"
    DEPENDS ${PRIMARY_FONT} ${SECONDARY_FONT} ${FONT_SCRIPT}
    COMMENT "Generating custom font with SF-Compact characters"
    USES_TERMINAL)

# Create a dummy custom command to represent the font header
add_custom_command(
    OUTPUT ${FONT_HEADER}
    DEPENDS generate_font
    COMMENT "Font header generated"
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${FONT_HEADER})

set(BOOT_BITMAP_HEADER ${PROJECT_BINARY_DIR}/boot.h)
set(BOOT_PNG ${PROJECT_SOURCE_DIR}/media/aeron2.png)
add_custom_command(
    OUTPUT ${BOOT_BITMAP_HEADER}
    COMMAND python3 ${PROJECT_SOURCE_DIR}/png_to_header.py -i ${BOOT_PNG} -o ${BOOT_BITMAP_HEADER} -n boot_bitmap
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS ${BOOT_PNG}
    USES_TERMINAL)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${BOOT_BITMAP_HEADER})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${CMAKE_PROJECT_NAME}> > ${CMAKE_PROJECT_NAME}.S
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf
    USES_TERMINAL)
