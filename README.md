# CHIP-8 Emulator:: Documentation
A simple emulator for CHIP-8

## Motivation

Hey friends, my name is Jody Gyekye and this is my project. For a long time, I have loved to play games and one of the most fascinating things I have used are emulators!
From a young age, I've always wanted to try and make some. However, I have very little experience with it at the moment.
So, here is my first emulation project: a CHIP-8 emulator.
There is no reason for me to pretend that I know what CHIP-8 is, however, I have learnt that it was one of the founding fathers for games (I think, don't crucify me).
So here it is!

If this does go public, I hope you will be able to enjoy it. I do want to personalise it a bit, so please bare with any oddly "personalised" features (if I gain such XP and choose to exercise it XD).

## Acknowledgements

I would like to thank these two pages for guidance. They helped give me an idea of what I should do and how I should proceed with those steps:
 - [Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
 - [Austin Morlan](https://austinmorlan.com/posts/chip8_emulator/#how-does-a-cpu-work)
## CHIP-8 Background

The CHIP-8 isn't technically a real machine, but a virtual machine (like JVM!). People would write programs for this VM, and an interpreter would "interpret" the instructions. Due to its "virtual" nature, the same program could be run on different machines, so long as they had an interpreter. The project is actually creating these interpreters, but emulator is a far more "understandable" word.

### Description

As a "machine", CHIP-8 has the following components:
- **16 8-bit registers**
    - Registers are a location on the CPU for storage.
    - Labeled V0 to VF
        - remember these are hex digits, so they count 0, 1, 2, 3, 4, ..., 8, 9, A, B, C, D, E, and F.
    - Can hold any value from 0x00 to 0xFF.
- **4096 Bytes of memory**
    - Address space is segmented into three sections:
        - 0x000 - 0x1FF
        - 0x050 - 0x0A0: storage space for 16 built-in characters (0 through F).
        - 0x200 - 0xFFF: Instructions from the ROM, anything left is free to use.
- **16-bit Index register**
    - A register used to store memory addresses for use in operations.
- **16-bit Program counter**
    - Program counters hold the address of the next instruction to execute.
    - It is 16 bits because it should be able to hold the maximum memory address (0xFFF)
- **16 level Stack**
    - Stacks help a CPU keep track of the order of execution when it calls into functions.
        - **CALL**: An instruction which causes the CPU to begin executing instructions in a different region of the program.
        - **RET**: An instruction that makes the program go back to when it hit the CALL instruction.
    - Stack functions:
        - **Push**: putting a PC onto the stack.
        - **Pop**: pulling a PC off of the stack.
- **8-bit Stack pointer**
    - A stack pointer points to where in the 16 levels of the stack our most recent value was placed.
- **8-bit Delay timer**
    - A simple timer used for timing.
    - If the timer value is zero, it stays zero, otherwise, it will decrement at a rate of 60Hz.
- **8-bit Sound timer**
    - A simple timer used for sound.
        - Similar behaviour to delay timer.
- **16 Input keys**
    - They are made to match the first 16 hex values: 0 through to F.
        - A key is either pressed or not pressed.
- **64x32 Monochrome Display memory**
    - A memory buffer used for storing the graphics to display.
        - 64 pixels wide and 32 pixels high.
        - The draw instruction iterates over each pixel in a sprite and XORs the sprite pixel and the display pixel.
        - 
- 
## What is an emulator?

An emulator is a software that reads original machine code instructions assembled for the target machine, _interprets_ them, and then replicates the functionality of the target machine on the host machine.
    - ROM files contain the instructions, the emulator reads them and then works to mimic the original machine's functionality on the host machine.

Origin:
    - late 16th century: from Latin aemulat- ‘rivalled, equalled’, from the verb aemulari, from aemulus ‘rival’.
