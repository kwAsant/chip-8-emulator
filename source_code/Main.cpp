#include "Chip8.hpp"
#include "Platform.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
    /**
     * @note Argument Check
     * @details Ensure the correct number of command-line arguments are provided.
     * Expecting three command-line arguments:
     *  Scale: An integer to scale the display size.
     *  Delay: An integer to set the delay between cycles (different games run best at different speeds, so we can control it here).
     *  ROM: The path to the ROM file to be loaded.
     */
    if (argc != 4) // Expecting 3 arguments: Scale, Delay, ROM
    {
        std::cerr << "Usage: " << argv[0] << " <Scale><Delay><ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    int scale = std::stoi(argv[1]);
    int delay = std::stoi(argv[2]);
    const char* romPath = argv[3];

    // Initialize the platform with scaled window size
    Platform platform("CHIP-8 Emulator", DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Initialize the Chip8 system and load the ROM
    Chip8 chip8;
    chip8.loadROM(romPath);

    int displayPitch = DISPLAY_WIDTH * sizeof(chip8.display[0]); // Calculate the pitch for the display update

    // Timing variables for controlling the cycle rate
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool running = true;

    while (running)
    {
        // Process input and update the keypad state
        running = !platform.ProcessInput(chip8.keypad); // because it is using a different type of flag which returns if the program should quit

        // Control the speed of execution based on the specified delay
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (elapsedTime > delay)
        {
            lastCycleTime = currentTime;
            chip8.Cycle(); // Execute one cycle of the Chip8 CPU
            platform.Update(chip8.display, displayPitch); // Update the display with the current framebuffer
        }
    }
    return 0;
}