#pragma once

#include <cstdint>

class Chip8 {
public:
    Chip8();
    void initialize();
    void loadProgram(const char* filename);
    void emulateCycle();
    void setKeys();

    bool drawFlag;

private:
    uint8_t memory[4096];
    uint8_t V[16]; 
    uint16_t I; 
    uint16_t pc; 
    uint8_t gfx[64 * 32]; // Graphics system
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t stack[16];
    uint16_t sp; // Stack pointer
    uint8_t key[16]; 

    void decodeAndExecuteOpcode(uint16_t opcode);
};