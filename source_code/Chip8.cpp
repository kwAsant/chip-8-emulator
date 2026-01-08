/**
 * @file Chip8.cpp
 * @brief File that contains the definition of the CHIP-8 class.
 * @author Jody Gyekye
 * @date June 2025
 * @version 1.0
 */

#include "Chip8.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <chrono>
#include <random>

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
// IMPLEMENTATION OF FETCH-DECODE-EXECUTE CYCLE WILL BE DONE IN THE NEXT PHASE
// =======================================================================================
void Chip8::Cycle()
{
    /**
     * @note Fetch
     * CHIP-8 opcodes are 2 bytes long, and are stored in big-endian format (MSB comes
     * first in memory, followed by LSB). The program counter (pc) points to the current
     * instruction in memory. To fetch the opcode, we read two consecutive bytes from
     * memory.
     */
    opcode = (memory[pc] << 8u) | memory[pc + 1]; // combine two bytes to form the opcode
    pc += 2; // increment the program counter to point to the next instruction

    /**
     * @note Decode and Execute
     * The first nibble (4 bits) of the opcode determines the type of instruction.
     * We use this nibble to index into a function pointer table (table) to find
     * the corresponding member function that implements the instruction.
     * We then call this function to execute the instruction.
     */
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))(); // decode and execute the opcode

    // decrement timers if they are set
    if (delayTimer > 0) {
        --delayTimer;
    }

    if (soundTimer > 0) {
        --soundTimer;
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

/**
 * @brief Instruction 8xy1: Sets VX to VX OR VY.
 * Performs a bitwise OR operation between the values in registers VX and VY, storing the result in VX.
 */
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    registers[Vx] |= registers[Vy]; // set VX to VX OR VY
}

/**
 * @brief Instruction 8xy2: Sets VX to VX AND VY.
 * Performs a bitwise AND operation between the values in registers VX and VY, storing the result in VX.
 */
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    registers[Vx] &= registers[Vy]; // set VX to VX AND VY
}

/**
 * @brief Instruction 8xy3: Sets VX to VX XOR VY.
 * Performs a bitwise XOR operation between the values in registers VX and VY, storing the result in VX.
 */
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    registers[Vx] ^= registers[Vy]; // set VX to VX XOR VY
}

/**
 * @brief Instruction 8xy4: Sets VX = VX + VY, set VF = carry.
 * Adds the values in registers VX and VY, storing the result in VX.
 * If there is a carry (i.e., the result exceeds 255), sets register VF to 1; otherwise, sets VF to 0.
 */
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    uint16_t sum = registers[Vx] + registers[Vy]; // use uint16_t to detect overflow

    if (sum > 255u) {
        registers[0xF] = 1; // set VF to 1 if there is a carry
    } else {
        registers[0xF] = 0; // set VF to 0 if there is no carry
    }

    registers[Vx] = sum & 0xFFu; // store the least significant 8 bits of the sum in VX
}

/**
 * @brief Instruction 8xy5: Sets VX = VX - VY, set VF = NOT borrow.
 * Subtracts the value in register VY from the value in register VX, storing the result in VX.
 * If VX is greater than or equal to VY, sets register VF to 1; otherwise, sets VF to 0.
 */
void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1; // set VF to 1 if no borrow
    } else {
        registers[0xF] = 0; // set VF to 0 if there is a borrow
    }

    registers[Vx] -= registers[Vy]; // subtract VY from VX
}

/**
 * @brief Instruction 8xy6: Sets VX = VX SHR 1.
 * Shifts the value in register VX to the right by 1 bit.
 * The least significant bit (LSB) of VX before the shift is stored in register VF.
 */
void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    registers[0xF] = registers[Vx] & 0x1u; // store the LSB of VX in VF
    registers[Vx] >>= 1; // shift VX to the right by 1 bit
}

/**
 * @brief Instruction 8xy7: Sets VX = VY - VX, set VF = NOT borrow.
 * Subtracts the value in register VX from the value in register VY, storing the result in VX.
 * If VY is greater than or equal to VX, sets register VF to 1; otherwise, sets VF to 0.
 */
void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    if (registers[Vy] > registers[Vx]) {
        registers[0xF] = 1; // set VF to 1 if no borrow
    } else {
        registers[0xF] = 0; // set VF to 0 if there is a borrow
    }

    registers[Vx] = registers[Vy] - registers[Vx]; // subtract VX from VY and store in VX
}

/**
 * @brief Instruction 8xyE: Sets VX = VX SHL 1.
 * Shifts the value in register VX to the left by 1 bit.
 * The most significant bit (MSB) of VX before the shift is stored in register VF.
 */
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u; // store the MSB of VX in VF
    registers[Vx] <<= 1; // shift VX to the left by 1 bit
}

/**
 * @brief Instruction 9xy0: Skips the next instruction if VX doesn't equal VY.
 * Compares the values in registers VX and VY.
 * If they are not equal, increments the program counter by 2 to skip the next instruction.
 */
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode

    if (registers[Vx] != registers[Vy]) {
        pc += 2; // skip next instruction
    }
}

/**
 * @brief Instruction Annn: Sets index to address NNN.
 * Loads the address NNN into the index register.
 */
void Chip8::OP_Annn() {
    uint16_t address = opcode & 0x0FFFu; // extract NNN from opcode

    index = address; // set index to address NNN
}

/**
 * @brief Instruction Bnnn: Jumps to address NNN + V0.
 * Sets the program counter to the address NNN plus the value in register V0.
 */
void Chip8::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFFu; // extract NNN from opcode

    pc = registers[0x0] + address; // set program counter to NNN + V0
}

/**
 * @brief Instruction Cxkk: Sets VX = random byte AND kk.
 * Generates a random byte, performs a bitwise AND with the immediate value kk, and stores the result in register VX.
 */
void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t byte = opcode & 0x00FFu; // extract kk from opcode

    registers[Vx] = randByte(randGen) & byte; // set VX to random byte AND kk
}

/**
 * @brief Instruction Dxyn: Displays n-byte sprite starting at memory location index at (VX, VY), set VF = collision.
 *      **A collision occurs if drawing a sprite causes any pixel that was already ON to be turned OFF.**
 * Draws a sprite at the coordinates (VX, VY) on the display.
 * The sprite is n bytes long and is located in memory starting at the address stored in the index register.
 * If any pixels are flipped from set to unset during the drawing process, sets register VF to 1; otherwise, sets VF to 0.
 */
void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // extract Y from opcode
    uint8_t height = opcode & 0x000Fu; // extract N from opcode

    uint8_t xPos = registers[Vx] % 64; // wrap around if xPos > 63 (screen width)
    uint8_t yPos = registers[Vy] % 32; // wrap around if yPos > 31 (screen height)

    registers[0xF] = 0; // reset VF (collision flag)

    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[index + row]; // get the sprite byte from memory

        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col); // extract the pixel from the sprite byte
            uint32_t* screenPixel = &display[(yPos + row) * 64 + (xPos + col)];

            if (spritePixel) { // if the pixel is set

                if (*screenPixel == 0xFFFFFFFF) { // if the screen pixel is already set
                    registers[0xF] = 1; // set VF to indicate a collision
                }

                *screenPixel ^= 0xFFFFFFFF; // toggle the pixel on the screen
            }
        }
    }
}

/**
 * @brief Instruction Ex9E: Skips the next instruction if key with the value of VX is pressed.
 * Checks if the key corresponding to the value in register VX is currently pressed.
 * If the key is pressed, increments the program counter by 2 to skip the next instruction.
 */
void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t key = registers[Vx];

    if (keypad[key]) { // if the key is pressed
        pc += 2; // skip next instruction
    }
}

/**
 * @brief Instruction ExA1: Skips the next instruction if key with the value of VX is not pressed.
 * Checks if the key corresponding to the value in register VX is currently not pressed.
 * If the key is not pressed, increments the program counter by 2 to skip the next instruction.
 */
void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t key = registers[Vx];

    if (!keypad[key]) { // if the key is not pressed
        pc += 2; // skip next instruction
    }
}

/**
 * @brief Instruction Fx07: Sets VX = delay timer value.
 * Loads the current value of the delay timer into register VX.
 */
void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    registers[Vx] = delayTimer; // set VX to delay timer value
}

/**
 * @brief Instruction Fx0A: Waits for a key press, store the value of the key in VX.
 * Pauses execution until a key is pressed.
 * Once a key is pressed, stores the value of the key in register VX.
 */
void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    bool keyPressed = false;

    // check if any key is pressed
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (keypad[i])
        {
            registers[Vx] = i;
            keyPressed = true;
            break;
        }
    }

    if (!keyPressed)
    {
        pc -= 2; // repeat this instruction
    }
}

/**
 * @brief Instruction Fx15: Sets delay timer = VX.
 * Sets the delay timer to the value in register VX.
 */
void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    delayTimer = registers[Vx]; // set delay timer to VX
}

/**
 * @brief Instruction Fx18: Sets sound timer = VX.
 * Sets the sound timer to the value in register VX.
 */
void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    soundTimer = registers[Vx]; // set sound timer to VX
}

/**
 * @brief Instruction Fx1E: Sets index = index + VX.
 * Adds the value in register VX to the index register.
 */
void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    index += registers[Vx]; // add VX to index
}

/**
 * @brief Instruction Fx29: Sets index = location of sprite for digit VX.
 * Sets the index register to the memory address of the sprite corresponding to the hexadecimal digit stored in register VX.
 */
void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t digit = registers[Vx];

    index = FONTSET_START_ADDRESS + (digit * 5); // set index to the address of the sprite for digit VX
}

/**
 * @brief Instruction Fx33: Stores BCD representation of VX in memory locations index, index+1, and index+2.
 * Converts the value in register VX to its binary-coded decimal (BCD) representation and stores it in memory.
 * The hundreds digit is stored at the address in the index register, the tens digit at index + 1, and the units digit at index + 2.
 */
void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode
    uint8_t value = registers[Vx];

    memory[index] = value / 100; // hundreds digit
    memory[index + 1] = (value / 10) % 10; // tens digit
    memory[index + 2] = value % 10; // units digit
}

/**
 * @brief Instruction Fx55: Stores registers V0 through VX in memory starting at location index.
 * Copies the values from registers V0 to VX into memory, starting at the address stored in the index register.
 */
void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    for (uint8_t i = 0; i <= Vx; ++i) {
        memory[index + i] = registers[i]; // store registers V0 to VX in memory
    }
}

/**
 * @brief Instruction Fx65: Reads registers V0 through VX from memory starting at location index.
 * Copies values from memory into registers V0 to VX, starting at the address stored in the index register.
 */
void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // extract X from opcode

    for (uint8_t i = 0; i <= Vx; ++i) {
        registers[i] = memory[index + i]; // read registers V0 to VX from memory
    }
}