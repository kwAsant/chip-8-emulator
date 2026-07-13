cmake_minimum_required(VERSION 3.14)
project(chip8)

set(CMAKE_CXX_STANDARD 11)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

# Output executable to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find SDL2
find_package(SDL2 REQUIRED)

add_library(chip8_core src/Chip8.cpp)
target_include_directories(chip8_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Executable
add_executable(chip8 src/Main.cpp src/Platform.cpp)
target_link_libraries(chip8 PRIVATE chip8_core SDL2::SDL2)

# Compile options
target_compile_options(chip8 PRIVATE -Wall)

include(CTest)
if (BUILD_TESTING)
    add_executable(chip8_tests tests/Chip8Tests.cpp)
    target_link_libraries(chip8_tests PRIVATE chip8_core)
    add_test(NAME chip8_core_tests COMMAND chip8_tests)
endif()

if (ENABLE_SANITIZERS)
    foreach(target chip8_core chip8)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endforeach()
    if (BUILD_TESTING)
        target_compile_options(chip8_tests PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(chip8_tests PRIVATE -fsanitize=address,undefined)
    endif()
endif()

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
