#include "chip8.h"
#include <fstream>
#include <iostream>

Chip8::Chip8() { }

void Chip8::initialize() {
    // Initialize once
    pc = 0x200; 
    opcode = 0; 
    I = 0;      
    sp = 0;     

    // Clear display
    // Clear stack
    // Clear registers V0-VF
    // Clear memory

    // Load fontset

    // Reset timers
}

void Chip8::loadProgram(const char* filename) {
    // Load the CHIP-8 program into memory
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        char* buffer = new char[size];
        if (file.read(buffer, size)) {
            for (int i = 0; i < size; ++i) {
                memory[0x200 + i] = buffer[i];
            }
        }
        delete[] buffer;
        file.close();
    }
    else {
        std::cerr << "Failed to open program file: " << filename << std::endl;
    }
}

void Chip8::emulateCycle() {
    // Fetch opcode
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode and execute opcode
    decodeAndExecuteOpcode(opcode);

    // Update timers
}

void Chip8::setKeys() {
    // Store key press state (press and release)
}

void Chip8::decodeAndExecuteOpcode(uint16_t opcode) {
    // Decode and execute opcode
}