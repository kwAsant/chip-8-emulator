#pragma once

/**
 * @file Chip8.hpp
 * @brief Header file that contains the definition of the CHIP-8 class.
 * @author Jody Gyekye
 * @date June 2025
 * @version 1.0
 */

#include <cstdint>
#include <random>

// define CONSTANTS
const unsigned int MEMORY_SIZE = 4096; // Total memory size of CHIP-8 in bytes
const unsigned int REGISTER_COUNT = 16; // Number of general-purpose registers
const unsigned int STACK_SIZE = 16;    // Maximum stack depth
const unsigned int KEYPAD_SIZE = 16;   // Number of keys on the CHIP-8 keypad
const unsigned int DISPLAY_WIDTH = 64; // Width of the CHIP-8 display in pixels
const unsigned int DISPLAY_HEIGHT = 32; // Height of the CHIP-8

// CHIP-8 Emulator Class Definition
// This class represents the state of a Chip8 emulator, including its registers, memory, timers, and display.
class Chip8
{
public:
    Chip8();                            // Constructor to initialize the Chip8 system
    void loadROM(const char *filename); // Load a ROM file into memory
    void Cycle();                // Execute one cycle of the fetch-decode-execute loop

    uint8_t keypad[KEYPAD_SIZE]{};        // 16 keys on the keypad
    uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{}; // 64x32 monochrome display memory
private:
    void Table0(); // Handle opcodes starting with 0
    void Table8(); // Handle opcodes starting with 8
    void TableE(); // Handle opcodes starting with E
    void TableF(); // Handle opcodes starting with F

    void OP_NULL(); // Placeholder for unimplemented opcodes
    void OP_00E0(); // Clear the display
    void OP_00EE(); // Return from subroutine
    void OP_1nnn(); // Jump to address NNN
    void OP_2nnn(); // Call subroutine at NNN
    void OP_3xkk(); // Skip next instruction if VX equals kk
    void OP_4xkk(); // Skip next instruction if VX doesn't equal kk
    void OP_5xy0(); // Skip next instruction if VX equals VY
    void OP_6xkk(); // Set VX to kk
    void OP_7xkk(); // Add kk to VX
    void OP_8xy0(); // Set VX to the value of VY
    void OP_8xy1(); // Set VX to VX OR VY
    void OP_8xy2(); // Set VX to VX AND VY
    void OP_8xy3(); // Set VX to VX XOR VY
    void OP_8xy4(); // Set VX = VX + VY, set VF = carry
    void OP_8xy5(); // Set VX = VX - VY, set VF = NOT borrow
    void OP_8xy6(); // Set VX = VX SHR 1
    void OP_8xy7(); // Set VX = VY - VX, set VF = NOT borrow
    void OP_8xyE(); // Set VX = VX SHL 1
    void OP_9xy0(); // Skip next instruction if VX doesn't equal VY
    void OP_Annn(); // Set index register to address NNN
    void OP_Bnnn(); // Jump to address NNN + V0
    void OP_Cxkk(); // Set VX = random byte AND kk
    void OP_Dxyn(); // Display n-byte sprite starting at memory location index at (VX, VY), set VF = collision
    void OP_Ex9E(); // Skip next instruction if key with the value of VX is pressed
    void OP_ExA1(); // Skip next instruction if key with the value of VX is not pressed
    void OP_Fx07(); // Set VX = delay timer value
    void OP_Fx0A(); // Wait for a key press, store the value of the key in VX
    void OP_Fx15(); // Set delay timer = VX
    void OP_Fx18(); // Set sound timer = VX
    void OP_Fx1E(); // Set index = index + VX
    void OP_Fx29(); // Set index = location of sprite for digit VX
    void OP_Fx33(); // Store BCD representation of VX in memory locations index, index+1, and index+2
    void OP_Fx55(); // Store registers V0 through VX in memory starting at location index
    void OP_Fx65(); // Read registers V0 through VX from memory starting at location index

    uint8_t registers[REGISTER_COUNT]{}; // 16 8-bit registers
    uint8_t memory[MEMORY_SIZE]{};  // 4096 bytes of memory
    uint16_t index{};        // 16-bit index register
    uint16_t pc{};           // 16-bit program counter
    uint16_t stack[STACK_SIZE]{};    // 16 level stack
    uint8_t sp{};            // stack pointer
    uint8_t delayTimer{};    // 8-bit delay timer
    uint8_t soundTimer{};    // 8-bit sound timer
    uint16_t opcode{};       // Current opcode

    std::default_random_engine randGen;              // Random number generator
    std::uniform_int_distribution<uint8_t> randByte; // Distribution to generate random bytes

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];   // the first digits of the opcode go from $0 to $F
    Chip8Func table0[0xE + 1];  // $0 need an array that can index up to $F+1
    Chip8Func table8[0xE + 1];  // $8 need an array that can index up to $F+1
    Chip8Func tableE[0xE + 1];  // $E need an array that can index up to $F+1
    Chip8Func tableF[0x65 + 1]; // $F need an array that can index up to $65+1
};