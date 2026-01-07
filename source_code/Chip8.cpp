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
#include <chrono>
#include <random>

// CHIP-8 Emulator Class Definition
// This class represents the state of a Chip8 emulator, including its registers, memory, timers, and display.
class Chip8 {
    public:
        Chip8(); // Constructor to initialize the Chip8 system
        void loadROM(const char* filename); // Load a ROM file into memory
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

        uint8_t registers[16]{};                         // 16 8-bit registers
        uint8_t memory[4096]{};                          // 4096 bytes of memory
        uint16_t index{};                                // 16-bit index register
        uint16_t pc{};                                   // 16-bit program counter
        uint16_t stack[16]{};                            // 16 level stack
        uint8_t sp{};                                    // stack pointer
        uint8_t delayTimer{};                            // 8-bit delay timer
        uint8_t soundTimer{};                            // 8-bit sound timer
        uint8_t keypad[16]{};                            // 16 keys on the keypad
        uint32_t display[64 * 32]{};                     // 64x32 monochrome display memory
        uint16_t opcode{};                               // Current opcode

        std::default_random_engine randGen;              // Random number generator
        std::uniform_int_distribution<uint8_t> randByte; // Distribution to generate random bytes


        typedef void (Chip8::*Chip8Func)();
        Chip8Func table[0xF + 1]; // the first digits of the opcode go from $0 to $F
        Chip8Func table0[0xE + 1]; // $0 need an array that can index up to $E+1
        Chip8Func table8[0xE + 1]; // $8 need an array that can index up to $E+1
        Chip8Func tableE[0xE + 1]; // $E need an array that can index up to $E+1
        Chip8Func tableF[0x65 + 1]; // $F need an array that can index up to $65+1
    };

// declare constants for CHIP-8
const unsigned int START_ADDRESS = 0x200; // Start address for CHIP-8 programs (first instruction is stored here)
const unsigned int FONTSET_SIZE = 80; // Size of the font set (in bytes)
const unsigned int FONTSET_START_ADDRESS = 0x50; // Start address for the font set in memory


uint8_t fontset[FONTSET_SIZE] = // CHIP-8 font set (each character is 5 bytes)
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
 * @brief Constructor for the Chip8 class.
 * Initializes the program counter to the start address.
 * Loads the font set into memory.
 * Initializes the random number generator.
 * Sets up the opcode function pointer table.
 */
Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())  
{
    pc = START_ADDRESS; // Program counter starts at 0x200 address

    // load the fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i]; // starting from 0x50
    }

    // initialise random number generator
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // set up the opcode function pointer table here in the next phase
    /**
     * @note The opcode function pointer table maps the first nibble of the opcode to the corresponding member function.
     * This allows for efficient decoding and execution of opcodes.
     * Each entry in the table is a pointer to a member function of the Chip8 class that implements the corresponding opcode.
     */
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++) {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;    
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::Table0() {
    ((*this).*(table0[opcode & 0x000Fu]))(); // call the appropriate function based on the last nibble
}

void Chip8::Table8() {
    ((*this).*(table8[opcode & 0x000Fu]))(); // call the appropriate function based on the last nibble
}

void Chip8::TableE() {
    ((*this).*(tableE[opcode & 0x000Fu]))(); // call the appropriate function based on the last nibble
}

void Chip8::TableF() {
    ((*this).*(tableF[opcode & 0x00FFu]))(); // call the appropriate function based on the last byte
}

void Chip8::OP_NULL() {}


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

// =======================================================================================
// THE INSTRUCTIONS FOR THE CHIP-8 EMULATOR WILL BE IMPLEMENTED IN THE NEXT PHASE
// Modern ISAs do (e.g. JMP, MOV, ADD), but CHIP-8 was:
//  • Designed in the 1970s
//  • Meant to be simple and compact
//  • Often documented in tables, not assemblers
// =======================================================================================

/**
 * @brief Instruction 00E0: Clear the display.
 * Clears the display by setting all pixels to 0.
 */
void Chip8::OP_00E0()
{
    memset(display, 0, sizeof(display)); // memmset sets a block of memory to a specific value
}

/**
 * @brief Instruction 00EE: Return from subroutine.
 * Sets the program counter to the address at the top of the stack and decrements the stack pointer.
 */
void Chip8::OP_00EE() {
    --sp; // decrement stack pointer
    pc = stack[sp]; // set program counter to the address at the top of the stack
}

/**
 * @brief Instruction 1nnn: Jumps to address NNN.
 * Sets the program counter to the address NNN.
 * Don't take a parameter since NNN is part of the opcode, and is less idiomatic to pass it as a parameter for emulators
 */
void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu; // setting type of address ensures type correctness (no implicit conversion issues)
    pc = address;
}

/**
 * @brief Instruction 2nnn: Calls subroutine at NNN.
 * Stores the current program counter on the stack and jumps to the address NNN.
 */
void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = pc; // store current pc in stack
    ++sp; // increment stack pointer
    pc = address; // set pc to the address NNN
}

/**
 * @brief Instruction 3xkk: Skips the next instruction if VX equals kk.
 * Compares the value in register VX with the immediate value kk.
 * If they are equal, increments the program counter by 2 to skip the next instruction.
 */
void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t byte = opcode & 0x00FFu; // extract kk from opcode

    if (registers[Vx] == byte) {
        pc += 2; // skip next instruction
    }
}

/**
 * @brief Instruction 4xkk: Skips the next instruction if VX doesn't equal kk.
 * Compares the value in register VX with the immediate value kk.
 * If they are not equal, increments the program counter by 2 to skip the next instruction.
 */
void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t byte = opcode & 0x00FFu; // extract kk from opcode

    if (registers[Vx] != byte) {
        pc += 2; // skip next instruction
    }
}

/**
 * @brief Instruction 5xy0: Skips the next instruction if VX equals VY.
 * Compares the values in registers VX and VY.
 * If they are equal, increments the program counter by 2 to skip the next instruction.
 */
void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    if (registers[Vx] == registers[Vy]) {
        pc += 2; // skip next instruction
    }
}

/**
 * @brief Instruction 6xkk: Sets VX to kk.
 * Loads the immediate value kk into register VX.
 */
void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t byte = opcode & 0x00FFu; // extract kk from opcode

    registers[Vx] = byte; // set VX to kk
}

/**
 * @brief Instruction 7xkk: Adds kk to VX (without carry).
 * Adds the immediate value kk to the value in register VX.
 */
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t byte = opcode & 0x00FFu; // extract kk from opcode

    registers[Vx] += byte; // add kk to VX
}

/**
 * @brief Instruction 8xy0: Sets VX to the value of VY.
 * Copies the value from register VY into register VX.
 */
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    registers[Vx] = registers[Vy]; // set VX to the value of VY
}

