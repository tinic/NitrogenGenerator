cmake_minimum_required(VERSION 3.16.3)

#set(CMAKE_C_COMPILER "/opt/homebrew/bin/gcc-14")
#set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-14")

set(CMAKE_CXX_STANDARD 26)

project(screen_test)

set(CMAKE_CXX_FLAGS "-Wall -Werror -fconstexpr-steps=33554432")

add_executable (screen_test screen_test.cpp ../screen.cpp ../mcp.cpp)


target_compile_definitions(screen_test PRIVATE SCREEN_TEST)
target_include_directories(screen_test PRIVATE "../" "../build" "./sixel-tools")

# Generate font for screen_test as well
set(FONT_HEADER ${CMAKE_BINARY_DIR}/ibmplexsans_regular_18_mono.hpp)
set(FONT_SCRIPT "${CMAKE_SOURCE_DIR}/../cmake/generate_font.cmake")

add_custom_target(generate_font_test ALL
    COMMAND ${CMAKE_COMMAND} 
        -DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}/.."
        -DCMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
        -P "${FONT_SCRIPT}"
    DEPENDS ${FONT_SCRIPT}
    COMMENT "Generating custom font for test"
    USES_TERMINAL)

add_custom_command(
    OUTPUT ${FONT_HEADER}
    DEPENDS generate_font_test
    COMMENT "Font header generated for test"
)

target_sources(screen_test PRIVATE ${FONT_HEADER})
