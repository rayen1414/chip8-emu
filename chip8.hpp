#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <cstdio>
#include <algorithm> 
#include <atomic>
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
    std::atomic<uint8_t> delay_timer{0};
    std::atomic<uint8_t> sound_timer{0};
    std::atomic<bool> isRunning{true}; // 
    unsigned char key[16]; // Stores state of keys

public:
    unsigned short gfx[2048]; //screen grid
    chip8();
    bool drawflag;
    void emulateCycle();
    void timer_loop();//timer
    void setKeyState(int keyIndex, bool isPressed);
    void loadfile(const std::string &filePath);
};

#endif