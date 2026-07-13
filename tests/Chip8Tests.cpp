#include "Chip8.hpp"

#include <cstdlib>
#include <iostream>

namespace {

void require(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void loadAndRun(Chip8& chip8, const uint8_t* program, size_t size, int cycles)
{
    require(chip8.LoadProgram(program, size), "program should fit in CHIP-8 memory");
    for (int i = 0; i < cycles; ++i) {
        chip8.Cycle();
    }
}

void testArithmetic()
{
    Chip8 chip8;
    const uint8_t program[] = {0x60, 0x0A, 0x70, 0x05};
    loadAndRun(chip8, program, sizeof(program), 2);
    require(chip8.RegisterValue(0) == 15, "7xkk should add an immediate value");
}

void testShiftQuirk()
{
    const uint8_t program[] = {0x61, 0x03, 0x62, 0x04, 0x81, 0x26};

    Chip8 modern;
    loadAndRun(modern, program, sizeof(program), 3);
    require(modern.RegisterValue(1) == 1, "modern shift should use Vx");
    require(modern.RegisterValue(0xF) == 1, "modern shift should store Vx bit 0 in VF");

    Chip8Quirks quirks;
    quirks.shiftUsesVy = true;
    Chip8 original(quirks);
    loadAndRun(original, program, sizeof(program), 3);
    require(original.RegisterValue(1) == 2, "original shift should use Vy");
    require(original.RegisterValue(0xF) == 0, "original shift should store Vy bit 0 in VF");
}

void testInvalidKeyDoesNotReadPastKeypad()
{
    Chip8 chip8;
    const uint8_t program[] = {0x60, 0xFF, 0xE0, 0x9E, 0x61, 0x01};
    loadAndRun(chip8, program, sizeof(program), 2);
    require(chip8.ProgramCounter() == 0x204, "invalid key value should not skip Ex9E instruction");
}

void testSpriteWrapping()
{
    Chip8 chip8;
    const uint8_t program[] = {0x60, 0x3F, 0x61, 0x1F, 0xA0, 0x50, 0xD0, 0x15};
    loadAndRun(chip8, program, sizeof(program), 4);
    require(chip8.display[31 * DISPLAY_WIDTH + 63] == 0xFFFFFFFF, "sprite should draw at bottom-right corner");
    require(chip8.display[31 * DISPLAY_WIDTH] == 0xFFFFFFFF, "sprite should wrap across right edge");
}

} // namespace

int main()
{
    testArithmetic();
    testShiftQuirk();
    testInvalidKeyDoesNotReadPastKeypad();
    testSpriteWrapping();
    std::cout << "All CHIP-8 core tests passed.\n";
    return EXIT_SUCCESS;
}
