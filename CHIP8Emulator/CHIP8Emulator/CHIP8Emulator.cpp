#include <iostream>
#include "chip8.h"

int main()
{
    Chip8 chip8;
    chip8.initialize();
    chip8.loadProgram("path_to_chip8_program.ch8");

    // Emulation loop
    for (;;) {
        chip8.emulateCycle();

        if (chip8.drawFlag) {
        }

        chip8.setKeys();
    }

    return 0;
}

