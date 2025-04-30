cmake_minimum_required(VERSION 3.16.3)

#set(CMAKE_C_COMPILER "/opt/homebrew/bin/gcc-14")
#set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-14")

set(CMAKE_CXX_STANDARD 26)

project(screen_test)

set(CMAKE_CXX_FLAGS "-Wall -Werror -fconstexpr-steps=33554432")

add_executable (screen_test screen_test.cpp ../screen.cpp ../mcp.cpp)

add_subdirectory(../magic_enum ${CMAKE_BINARY_DIR}/magic_enum EXCLUDE_FROM_ALL)
target_link_libraries(screen_test magic_enum::magic_enum)

add_subdirectory(../fixed-containers ${CMAKE_BINARY_DIR}/fixed-containers EXCLUDE_FROM_ALL)
target_link_libraries(screen_test fixed_containers::fixed_containers)

target_compile_definitions(screen_test PRIVATE SCREEN_TEST)
target_include_directories(screen_test PRIVATE "../" "../build/Debug" "./sixel-tools")
