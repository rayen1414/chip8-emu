#include "chip8.hpp"
#include <iterator>
#include <algorithm>
#include <cstdlib> // For rand()
#include <chrono>
#include <iostream>
#include <thread>
#include<fstream>
using namespace std::chrono;
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

    // Reset memory, registers, stack, graphics, and keys
    std::fill(std::begin(memory), std::end(memory), 0);
    std::fill(std::begin(V), std::end(V), 0);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(key), std::end(key), 0);
    std::fill(std::begin(gfx), std::end(gfx), 0);


    // Load font into memory (0x00 - 0x50)
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }
    timer_loop();
}
void chip8::setKeyState(int keyIndex, bool isPressed) {
        if (keyIndex >= 0 && keyIndex < 16) {
        key[keyIndex] = isPressed ? 1 : 0;
    }

}
void chip8::timer_loop() {
    std::thread t([this]() {
        auto time = 16666us;
        while (isRunning) {
            std::this_thread::sleep_for(time);
            
            // Decrement at 60 Hz in background
            if (delay_timer > 0) delay_timer--;
            if (sound_timer > 0) sound_timer--;
        }
    });
    t.detach();
}
void chip8::emulateCycle() {
    // Fetch 2-byte opcode
    opcode = (memory[pc] << 8) | memory[pc + 1];

    //  variable 
    unsigned char x = (opcode & 0x0F00) >> 8;
    unsigned char y = (opcode & 0x00F0) >> 4;
    unsigned char nn = opcode & 0x00FF;
    unsigned short nnn = opcode & 0x0FFF;

    // Decode opcode
    switch (opcode & 0xF000) {
        
        case 0x0000:
            switch (opcode & 0x00FF) {

                case 0x00EE: // Return from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;

        case 0x1000: // Jump to NNN
            pc = nnn;
            break;

        case 0x2000: // Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = nnn;
            break;

        case 0x3000: // Skip next if VX == NN
            if (V[x] == nn) pc += 4;
            else pc += 2;
            break;

        case 0x4000: // Skip next if VX != NN
            if (V[x] != nn) pc += 4;
            else pc += 2;
            break;

        case 0x5000: // Skip next if VX == VY
            if (V[x] == V[y]) pc += 4;
            else pc += 2;
            break;

        case 0x6000: // Set VX = NN
            V[x] = nn;
            pc += 2;
            break;

        case 0x7000: // Add NN to VX
            V[x] += nn;
            pc += 2;
            break;

        case 0x8000: // Math instructions
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
            break;

        case 0x9000: // Skip next if VX != VY
            if (V[x] != V[y]) pc += 4;
            else pc += 2;
            break;

        case 0xA000: // Set I = NNN
            I = nnn;
            pc += 2;
            break;

        case 0xB000: // Jump to NNN + V0
            pc = nnn + V[0];
            break;

        case 0xC000: // VX = rand() & NN
            V[x] = (rand() % 256) & nn;
            pc += 2;
            break;
    case 0xE000:
    switch (opcode & 0x00FF) {
        case 0x009E: // Skip next instruction if key in V[x] is pressed
            if (key[V[x]] != 0) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x00A1: // Skip next instruction if key in V[x] is NOT pressed
            if (key[V[x]] == 0) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
    }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // VX = delay_timer
                    V[x] = delay_timer;
                    pc += 2;
                    break;


                case 0x0015: // delay_timer = VX
                    delay_timer = V[x];
                    pc += 2;
                    break;

                case 0x0018: // sound_timer = VX
                    sound_timer = V[x];
                    pc += 2;
                    break;

                case 0x001E: // I += VX
                    I += V[x];
                    pc += 2;
                    break;

                case 0x0029: // Set I to font character location for VX
                    I = V[x] * 5;
                    pc += 2;
                    break;

                case 0x0033: // Store BCD of VX at I, I+1, I+2
                    if (I < 4094) {
                        memory[I]     = V[x] / 100;
                        memory[I + 1] = (V[x] / 10) % 10;
                        memory[I + 2] = V[x] % 10;
                    }
                    pc += 2;
                    break;

                case 0x0055: // Dump V0 thru VX into memory starting at I
                    for (int i = 0; i <= x; ++i) {
                        memory[I + i] = V[i];
                    }
                    pc += 2;
                    break;

                case 0x0065: // Load V0 thru VX from memory starting at I
                    for (int i = 0; i <= x; ++i) {
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;
                case 0x000A: // Wait for a key press, store key in V[x]
                 {
                    bool key_pressed = false;

                    for (int i = 0; i < 16; ++i) {
                        if (key[i] != 0) {
                            V[x] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                    if (!key_pressed) {
                        return; 
                    }

                    pc += 2;
                }
                break;
                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;
        case 0xD000:{// Draw sprite at (VX, VY) with height N
            unsigned char height = opcode & 0x000F;
            V[0xF] = 0;
            for(int yline=0;yline<height;yline++){
                for(int xline=0 ;xline<8;xline++){
                    int xPos = (V[x] + xline)%64;
                    int yPos = (V[y] + yline) % 32;
                if((memory[I+yline] & (0x80 >> xline)) != 0){
                if (gfx[xPos + (yPos * 64)] == 1) {
                    V[0xF] = 1;//collision
                }
            
                gfx[xPos + (yPos * 64)] ^= 1;
                drawflag= true;
                }
            }
            }
            pc+=2;
            break;
        }

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }


}
void chip8::loadfile(const std::string& filePath){
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << " Could not open file: " << filePath << std::endl;
        return;
    }
    std::streamsize filesize = file.tellg();
    if (filesize <= 0 || filesize > (4096 - 0x200)) {
        std::cerr << "Invalid ROM size (" << filesize << " bytes). Fits max 3584 bytes." << std::endl;
        file.close();
        return;
    }
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[0x200]), filesize);
    file.close();

}