#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <cstdio>
#include <algorithm> 
#include <string>
#include <cstdint>
class chip8
{
private:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];   // Return addresses
    unsigned short sp;          // Stack pointer
    // Timers 
    unsigned char key[16]; // Stores state of keys

public:
    uint8_t gfx[2048]; //screen grid
    chip8();
    bool drawflag;
    uint8_t delay_timer;
    uint8_t sound_timer;
    void emulateCycle();
    void setKeyState(int keyIndex, bool isPressed);
    bool loadfile(const std::string &filePath);
};

#endif