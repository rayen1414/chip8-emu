#include "chip8.hpp"
#include <iterator>
#include <algorithm>

// Font data for digits 0-F (5 bytes each)
const unsigned char chip8_fontset[80] = { 
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

chip8::chip8() {
    pc          = 0x200; // Programs start at 0x200
    opcode      = 0;     
    I           = 0;     
    sp          = 0;     
    delay_timer = 0;     
    sound_timer = 0;

    // Reset memory and registers
    std::fill(std::begin(memory), std::end(memory), 0);
    std::fill(std::begin(V), std::end(V), 0);
    std::fill(std::begin(stack), std::end(stack), 0);

    // Load font into memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }
}

void chip8::emulateCycle() {
    // Fetch 2-byte opcode
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Decode opcode
    switch (opcode & 0xF000) {
        
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // Clear screen
                    pc += 2;
                    break;

                case 0x000E: // Return from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;

        case 0x1000: // Jump to NNN
            pc = opcode & 0x0FFF;
            break;

        case 0x2000: // Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x6000: // Set VX = NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        case 0x7000: // Add NN to VX
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;

        case 0x8000: // Math instructions
            {
                unsigned char x = (opcode & 0x0F00) >> 8;
                unsigned char y = (opcode & 0x00F0) >> 4;

                switch (opcode & 0x000F) {
                    case 0x0000: // VX = VY
                        V[x] = V[y]; 
                        pc += 2;
                        break;

                    case 0x0001: // VX = VX | VY
                        V[x] |= V[y]; 
                        pc += 2;
                        break;

                    case 0x0002: // VX = VX & VY
                        V[x] &= V[y]; 
                        pc += 2;
                        break;

                    case 0x0003: // VX = VX ^ VY
                        V[x] ^= V[y]; 
                        pc += 2;
                        break;

                    case 0x0004: // VX += VY with carry
                        {
                            unsigned char carry = (V[y] > (0xFF - V[x])) ? 1 : 0;
                            V[x] += V[y];
                            V[0xF] = carry;
                            pc += 2;
                        }
                        break;

                    case 0x0005: // VX -= VY
                        {
                            unsigned char not_borrow = (V[x] >= V[y]) ? 1 : 0;
                            V[x] -= V[y];
                            V[0xF] = not_borrow;
                            pc += 2;
                        }
                        break;

                    case 0x0006: // VX >> 1
                        V[0xF] = V[x] & 0x1;
                        V[x] >>= 1;
                        pc += 2;
                        break;

                    case 0x0007: // VX = VY - VX
                        {
                            unsigned char not_borrow = (V[y] >= V[x]) ? 1 : 0;
                            V[x] = V[y] - V[x];
                            V[0xF] = not_borrow;
                            pc += 2;
                        }
                        break;

                    case 0x000E: // VX << 1
                        V[0xF] = (V[x] & 0x80) >> 7;
                        V[x] <<= 1;
                        pc += 2;
                        break;

                    default:
                        printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
                }
            }
            break;

        case 0xA000: // Set I = NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000: // Jump to NNN + V0
            pc = (opcode & 0x0FFF) + V[0];
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0033: // Store BCD of VX at I, I+1, I+2
                    if (I < 4094) {
                        memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
                        memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                        memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    }
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    // Decrement timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}