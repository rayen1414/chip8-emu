#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <cstdio>
#include <algorithm> 

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
    unsigned short gfx[2048]; //screen grid
    // Timers 
    unsigned char delay_timer; 
    unsigned char sound_timer; 
    unsigned char key[16]; // Stores state of keys

public:
    chip8();
    void emulateCycle();
    void timer_loop();//timer
    void setKeyState(int keyIndex, bool isPressed);
};

#endif