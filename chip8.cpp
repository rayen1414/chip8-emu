#include "chip8.hpp"
#include <iterator>
#include <algorithm>
// Built-in Fontset array (80 bytes)
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
    pc          = 0x200; // Program counter starts at 0x200
    opcode      = 0;     
    I           = 0;     
    sp          = 0;     
    delay_timer = 0;     // Timers start at 0
    sound_timer = 0;

    // Clear memory, registers, and stack completely
    std::fill(std::begin(memory), std::end(memory), 0);
    std::fill(std::begin(V), std::end(V), 0);
    std::fill(std::begin(stack), std::end(stack), 0);

    // Load fontset into start of memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }
}

void chip8::emulateCycle() {
    // 1. Fetch opcode (merge two 8-bit bytes into one 16-bit opcode)
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // 2. Decode & Execute
    switch (opcode & 0xF000) {
        
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // 0x00E0: Clear screen
                    // TODO: memset / clear display buffer
                    pc += 2;
                    break;

                case 0x000E: // 0x00EE: Return from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;

        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x8000: // 0x8XY4: Arithmetic operations
            switch (opcode & 0x000F) {
                case 0x0004: // VX += VY with carry
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; // Carry
                    else
                        V[0xF] = 0; // No carry

                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }
            break;

        case 0xA000: // 0xANNN: Set I to NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xF000: // 0xFX33: BCD and other F-family opcodes
            switch (opcode & 0x00FF) { // Match last two hex digits
                case 0x0033: // 0xFX33: Binary-coded decimal
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

    // 3. Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}