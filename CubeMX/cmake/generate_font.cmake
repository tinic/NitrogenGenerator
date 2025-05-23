# CMake script to generate custom font with SF-Compact characters
# This replaces the bash script and provides Windows compatibility

set(FONT_SIZE 18)
set(PRIMARY_FONT "${PROJECT_SOURCE_DIR}/constixel/genfonts/otf/IBMPlexSans-Regular.otf")
set(SECONDARY_FONT "${PROJECT_SOURCE_DIR}/otf/SF-Compact-Display-Regular.otf")
set(OUTPUT_NAME "IBMPlexSans-Regular_${FONT_SIZE}_custom")
set(CHARS "32-126")  # ASCII range
set(CUSTOM_CHARS "0x1001E4,0x10042B,0x10042F,0x000B2,0x26A0")  # Unicode chars from SF-Compact
set(FONTBM_DIR "${PROJECT_SOURCE_DIR}/constixel/genfonts")
set(TEMP_BUILD_DIR "${FONTBM_DIR}/temp_build")
set(FONT_HEADER "${CMAKE_BINARY_DIR}/ibmplexsans_regular_18_mono.hpp")

message(STATUS "Generating custom font: ${OUTPUT_NAME}")
message(STATUS "  Primary font: ${PRIMARY_FONT}")
message(STATUS "  Secondary font: ${SECONDARY_FONT}")
message(STATUS "  Font size: ${FONT_SIZE}")
message(STATUS "  Base chars: ${CHARS}")
message(STATUS "  Custom chars: ${CUSTOM_CHARS}")

# Check if font files exist
if(NOT EXISTS "${PRIMARY_FONT}")
    message(FATAL_ERROR "Primary font file not found: ${PRIMARY_FONT}")
endif()
if(NOT EXISTS "${SECONDARY_FONT}")
    message(FATAL_ERROR "Secondary font file not found: ${SECONDARY_FONT}")
endif()

# Build fontbm if needed
set(FONTBM_EXECUTABLE "${FONTBM_DIR}/fontbm/build/fontbm")
if(WIN32)
    set(FONTBM_EXECUTABLE "${FONTBM_EXECUTABLE}.exe")
endif()

if(NOT EXISTS "${FONTBM_EXECUTABLE}")
    message(STATUS "Building fontbm...")
    file(MAKE_DIRECTORY "${FONTBM_DIR}/fontbm/build")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release "${FONTBM_DIR}/fontbm"
        WORKING_DIRECTORY "${FONTBM_DIR}/fontbm/build"
        RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to configure fontbm")
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY "${FONTBM_DIR}/fontbm/build"
        RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to build fontbm")
    endif()
    message(STATUS "fontbm built successfully")
endif()

# Build genfont if needed
set(GENFONT_EXECUTABLE "${FONTBM_DIR}/build/genfont")
if(WIN32)
    set(GENFONT_EXECUTABLE "${GENFONT_EXECUTABLE}.exe")
endif()

if(NOT EXISTS "${GENFONT_EXECUTABLE}")
    message(STATUS "Building genfont...")
    file(MAKE_DIRECTORY "${FONTBM_DIR}/build")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release "${FONTBM_DIR}"
        WORKING_DIRECTORY "${FONTBM_DIR}/build"
        RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to configure genfont")
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY "${FONTBM_DIR}/build"
        RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to build genfont")
    endif()
    message(STATUS "genfont built successfully")
endif()

# Create temporary directory
file(MAKE_DIRECTORY "${TEMP_BUILD_DIR}")

# Generate font with fontbm
message(STATUS "Generating monochrome font with secondary font for special characters...")
execute_process(
    COMMAND "${FONTBM_EXECUTABLE}"
        --font-file "${PRIMARY_FONT}"
        --secondary-font-file "${SECONDARY_FONT}"
        --font-size "${FONT_SIZE}"
        --chars "${CHARS},${CUSTOM_CHARS}"
        --output "${TEMP_BUILD_DIR}/${OUTPUT_NAME}"
        --kerning-pairs extended
        --monochrome
        --force-auto-hinter
        --extra-info
        --texture-crop-width
        --texture-crop-height
        --tabular-numbers
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Failed to generate font with fontbm")
endif()

# Create build directory if it doesn't exist
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}")

# Generate the C++ header
message(STATUS "Generating header file...")
execute_process(
    COMMAND "${GENFONT_EXECUTABLE}"
        --font-file "${TEMP_BUILD_DIR}/${OUTPUT_NAME}.fnt"
        --out-dir "${CMAKE_BINARY_DIR}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Failed to generate header with genfont")
endif()

# Find the generated file and rename it
file(GLOB GENERATED_FILES "${CMAKE_BINARY_DIR}/*${FONT_SIZE}*custom*mono.hpp")
list(LENGTH GENERATED_FILES NUM_FILES)
if(NUM_FILES EQUAL 0)
    message(FATAL_ERROR "Could not find expected generated file")
endif()
list(GET GENERATED_FILES 0 ACTUAL_FILE)
message(STATUS "Found generated file: ${ACTUAL_FILE}")
message(STATUS "Moving to target location...")

file(RENAME "${ACTUAL_FILE}" "${FONT_HEADER}")

# Fix the class name in the generated file
file(READ "${FONT_HEADER}" FONT_CONTENT)
string(REPLACE "ibmplexsans_regular_18_custom_mono" "ibmplexsans_regular_18_mono" FONT_CONTENT "${FONT_CONTENT}")
file(WRITE "${FONT_HEADER}" "${FONT_CONTENT}")

# Clean up temporary files
file(REMOVE_RECURSE "${TEMP_BUILD_DIR}")

message(STATUS "Font generation complete!")
message(STATUS "Generated: ${FONT_HEADER}")