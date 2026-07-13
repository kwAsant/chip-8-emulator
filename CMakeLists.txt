cmake_minimum_required(VERSION 3.14)
project(chip8)

set(CMAKE_CXX_STANDARD 11)

# Output executable to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find SDL2
find_package(SDL2 REQUIRED)

# Executable
add_executable(
    chip8
    src/Chip8.cpp
    src/Main.cpp
    src/Platform.cpp
)

# Compile options
target_compile_options(chip8 PRIVATE -Wall)

# Link SDL2
target_link_libraries(chip8 PRIVATE SDL2::SDL2)

# Optional clean/rebuild targets
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMENT "Removing all build files"
)

add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning and rebuilding the project"
)