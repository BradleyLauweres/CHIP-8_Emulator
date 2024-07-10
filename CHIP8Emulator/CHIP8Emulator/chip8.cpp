#include "chip8.h"
#include <fstream>
#include <iostream>

uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, 
    0x20, 0x60, 0x20, 0x20, 0x70, 
    0xF0, 0x10, 0xF0, 0x80, 0xF0, 
    0xF0, 0x10, 0xF0, 0x10, 0xF0, 
    0x90, 0x90, 0xF0, 0x10, 0x10, 
    0xF0, 0x80, 0xF0, 0x10, 0xF0, 
    0xF0, 0x80, 0xF0, 0x90, 0xF0, 
    0xF0, 0x10, 0x20, 0x40, 0x40, 
    0xF0, 0x90, 0xF0, 0x90, 0xF0, 
    0xF0, 0x90, 0xF0, 0x10, 0xF0, 
    0xF0, 0x90, 0xF0, 0x90, 0x90, 
    0xE0, 0x90, 0xE0, 0x90, 0xE0, 
    0xF0, 0x80, 0x80, 0x80, 0xF0, 
    0xE0, 0x90, 0x90, 0x90, 0xE0, 
    0xF0, 0x80, 0xF0, 0x80, 0xF0, 
    0xF0, 0x80, 0xF0, 0x80, 0x80  
};

Chip8::Chip8() { }

Chip8::~Chip8()
{
}

void Chip8::initialize() {
    // Initialize once
    pc = 0x200; 
    opcode = 0; 
    I = 0;      
    sp = 0;     

    // Clear Everything or somethign idk
    memset(gfx, 0, sizeof(gfx));
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(memory, 0, sizeof(memory));

    // Load fontset
    //Todo
     
    delay_timer = 0;
    sound_timer = 0;
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
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    decodeAndExecuteOpcode(opcode);
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) --sound_timer;
}

void Chip8::setKeys() {

}

void Chip8::decodeAndExecuteOpcode(uint16_t opcode) {
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t  nn = opcode & 0x00FF;
    uint8_t  n = opcode & 0x000F;
    uint8_t  x = (opcode & 0x0F00) >> 8;
    uint8_t  y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode) {
        case 0x00E0: // 00E0: Clears the screen
            memset(gfx, 0, sizeof(gfx));
            drawFlag = true;
            pc += 2;
            break;
        case 0x00EE: // 00EE: Returns from subroutine
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
        default:
            std::cerr << "Unknown opcode [0x0000]: 0x" << std::hex << opcode << std::endl;
        }
        break;
    case 0x1000: // 1nnn: Jumps to address nnn
        pc = nnn;
        break;
    case 0x2000: // 2nnn: Calls subroutine at nnn
        stack[sp] = pc;
        ++sp;
        pc = nnn;
        break;
    case 0x3000: // 3xnn: Skips the next instruction if Vx equals nn
        if (V[x] == nn)
            pc += 4;
        else
            pc += 2;
        break;
    case 0x4000: // 4xnn: Skips the next instruction if Vx doesn't equal nn
        if (V[x] != nn)
            pc += 4;
        else
            pc += 2;
        break;
    case 0x5000: // 5xy0: Skips the next instruction if Vx equals Vy
        if (V[x] == V[y])
            pc += 4;
        else
            pc += 2;
        break;
    case 0x6000: // 6xnn: Sets Vx to nn
        V[x] = nn;
        pc += 2;
        break;
    case 0x7000: // 7xnn: Adds nn to Vx
        V[x] += nn;
        pc += 2;
        break;
    case 0x8000:
        switch (opcode & 0x000F) {
        case 0x0000: // 8xy0: Sets Vx to the value of Vy
            V[x] = V[y];
            pc += 2;
            break;
        case 0x0001: // 8xy1: Sets Vx to Vx OR Vy
            V[x] |= V[y];
            pc += 2;
            break;
        case 0x0002: // 8xy2: Sets Vx to Vx AND Vy
            V[x] &= V[y];
            pc += 2;
            break;
        case 0x0003: // 8xy3: Sets Vx to Vx XOR Vy
            V[x] ^= V[y];
            pc += 2;
            break;
        case 0x0004: // 8xy4: Adds Vy to Vx. VF is set to 1 when there's a carry, and to 0 when there isn't
            V[0xF] = (V[x] + V[y] > 0xFF) ? 1 : 0;
            V[x] += V[y];
            pc += 2;
            break;
        case 0x0005: // 8xy5: Vy is subtracted from Vx. VF is set to 0 when there's a borrow, and 1 when there isn't
            V[0xF] = (V[x] > V[y]) ? 1 : 0;
            V[x] -= V[y];
            pc += 2;
            break;
        case 0x0006: // 8xy6: Shifts Vx right by one. VF is set to the value of the least significant bit of Vx before the shift
            V[0xF] = V[x] & 0x1;
            V[x] >>= 1;
            pc += 2;
            break;
        case 0x0007: // 8xy7: Sets Vx to Vy minus Vx. VF is set to 0 when there's a borrow, and 1 when there isn't
            V[0xF] = (V[y] > V[x]) ? 1 : 0;
            V[x] = V[y] - V[x];
            pc += 2;
            break;
        case 0x000E: // 8xyE: Shifts Vx left by one. VF is set to the value of the most significant bit of Vx before the shift
            V[0xF] = V[x] >> 7;
            V[x] <<= 1;
            pc += 2;
            break;
        default:
            std::cerr << "Unknown opcode [0x8000]: 0x" << std::hex << opcode << std::endl;
        }
        break;
    case 0x9000: // 9xy0: Skips the next instruction if Vx doesn't equal Vy
        if (V[x] != V[y])
            pc += 4;
        else
            pc += 2;
        break;
    case 0xA000: // Annn: Sets I to the address nnn
        I = nnn;
        pc += 2;
        break;
    case 0xB000: // Bnnn: Jumps to the address nnn plus V0
        pc = nnn + V[0];
        break;
    case 0xC000: // Cxnn: Sets Vx to a random number and nn
        V[x] = (rand() % 0xFF) & nn;
        pc += 2;
        break;
    case 0xD000: // Dxyn: Draws a sprite at coordinate (Vx, Vy) that has a width of 8 pixels and a height of n pixels.
    {
        uint8_t xPos = V[x];
        uint8_t yPos = V[y];
        uint8_t height = n;
        uint8_t pixel;

        V[0xF] = 0;
        for (int yline = 0; yline < height; yline++) {
            pixel = memory[I + yline];
            for (int xline = 0; xline < 8; xline++) {
                if ((pixel & (0x80 >> xline)) != 0) {
                    if (gfx[(xPos + xline + ((yPos + yline) * 64))] == 1) {
                        V[0xF] = 1;
                    }
                    gfx[xPos + xline + ((yPos + yline) * 64)] ^= 1;
                }
            }
        }
        drawFlag = true;
        pc += 2;
    }
    break;
    case 0xE000:
        switch (opcode & 0x00FF) {
        case 0x009E: // Ex9E: Skips the next instruction if the key stored in Vx is pressed
            if (key[V[x]] != 0)
                pc += 4;
            else
                pc += 2;
            break;
        case 0x00A1: // ExA1: Skips the next instruction if the key stored in Vx isn't pressed
            if (key[V[x]] == 0)
                pc += 4;
            else
                pc += 2;
            break;
        default:
            std::cerr << "Unknown opcode [0xE000]: 0x" << std::hex << opcode << std::endl;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF) {
        case 0x0007: // Fx07: Sets Vx to the value of the delay timer
            V[x] = delay_timer;
            pc += 2;
            break;
        case 0x000A: // Fx0A: A key press is awaited, and then stored in Vx
        {
            bool keyPress = false;

            for (int i = 0; i < 16; ++i) {
                if (key[i] != 0) {
                    V[x] = i;
                    keyPress = true;
                }
            }

            // If we didn't receive a keypress, skip this cycle and try again.
            if (!keyPress)
                return;

            pc += 2;
        }
        break;
        case 0x0015: // Fx15: Sets the delay timer to Vx
            delay_timer = V[x];
            pc += 2;
            break;
        case 0x0018: // Fx18: Sets the sound timer to Vx
            sound_timer = V[x];
            pc += 2;
            break;
        case 0x001E: // Fx1E: Adds Vx to I. VF is not affected.
            I += V[x];
            pc += 2;
            break;
        case 0x0029: // Fx29: Sets I to the location of the sprite for the character in Vx. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
            I = V[x] * 0x5 + 0x50;
            pc += 2;
            break;
        case 0x0033: // Fx33: Stores the binary-coded decimal representation of Vx in memory locations I, I+1, and I+2.
            memory[I] = V[x] / 100;
            memory[I + 1] = (V[x] / 10) % 10;
            memory[I + 2] = (V[x] % 100) % 10;
            pc += 2;
            break;
        case 0x0055: // Fx55: Stores V0 to Vx in memory starting at address I.
            for (int i = 0; i <= x; ++i)
                memory[I + i] = V[i];

            I += x + 1;
            pc += 2;
            break;
        case 0x0065: // Fx65: Fills V0 to Vx with values from memory starting at address I.
            for (int i = 0; i <= x; ++i)
                V[i] = memory[I + i];

            I += x + 1;
            pc += 2;
            break;
        default:
            std::cerr << "Unknown opcode [0xF000]: 0x" << std::hex << opcode << std::endl;
        }
        break;
    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
    }
}
