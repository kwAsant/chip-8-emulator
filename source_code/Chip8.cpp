/**
 * @file Chip8.cpp
 * @brief File that contains the definition of the CHIP-8 class.
 * @author Jody Gyekye
 * @date June 2025
 * @version 1.0
 */

#include <cstdint>
#include <fstream>
#include <iostream>

// CHIP-8 Emulator Class Definition
// This class represents the state of a Chip8 emulator, including its registers, memory, timers, and display.
class Chip8 {
    public:
        uint8_t registers[16]{}; // 16 8-bit registers
        uint8_t memory[4096]{}; // 4096 bytes of memory
        uint16_t index{}; // 16-bit index register
        uint16_t pc{}; // 16-bit program counter
        uint16_t stack[16]{}; // 16 level stack
        uint8_t sp{}; //stack pointer
        uint8_t delayTimer{}; // 8-bit delay timer
        uint8_t soundTimer{}; // 8-bit sound timer
        uint8_t keypad[16]{}; // 16 keys on the keypad
        uint32_t display[64 * 32]{}; // 64x32 monochrome display memory
        uint16_t opcode{}; // Current opcode

        void loadROM(const char* filename); // Load a ROM file into memory
};

// declare constants for CHIP-8
const unsigned int START_ADDRESS = 0x200; // Start address for CHIP-8 programs (first instruction is stored here)

/**
 * @brief loads the contents of a ROM file into the CHIP-8 memory.
 * @param filename stores the name of the ROM file to be loaded.
 * @note `std::ios::ate` is used to open the file and move the read pointer to the end of the file.
 */
void Chip8::loadROM(const char* filename) {
    // open the ROM file in binary mode and move the read pointer to the end of the file
    //  this is because of the BITWISE OR which will perform the operation of both flags
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        // get the size of the file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        // go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // load the ROM contents into the CHIP-8 memory starting from the start address
        for (long i = 0; i < size; i++)
            memory[START_ADDRESS + i] = buffer[i];
        delete[] buffer; // free the allocated buffer
    }
    else {
        // if the file could not be opened, print an error message
        std::cerr << "Error: Could not open ROM file " << filename << std::endl;
    }
}