#include "chip8.hpp"
#include <iostream>
void processInput(chip8& c8, int keyboardKey, bool isPressed) {
    switch (keyboardKey) {
        case KEY_1: c8.setKeyState(0x1, isPressed); break;
        case KEY_2: c8.setKeyState(0x2, isPressed); break;
        case KEY_3: c8.setKeyState(0x3, isPressed); break;
        case KEY_4: c8.setKeyState(0xC, isPressed); break;

        case KEY_Q: c8.setKeyState(0x4, isPressed); break;
        case KEY_W: c8.setKeyState(0x5, isPressed); break;
        case KEY_E: c8.setKeyState(0x6, isPressed); break;
        case KEY_R: c8.setKeyState(0xD, isPressed); break;

        case KEY_A: c8.setKeyState(0x7, isPressed); break;
        case KEY_S: c8.setKeyState(0x8, isPressed); break;
        case KEY_D: c8.setKeyState(0x9, isPressed); break;
        case KEY_F: c8.setKeyState(0xE, isPressed); break;

        case KEY_Z: c8.setKeyState(0xA, isPressed); break;
        case KEY_X: c8.setKeyState(0x0, isPressed); break;
        case KEY_C: c8.setKeyState(0xB, isPressed); break;
        case KEY_V: c8.setKeyState(0xF, isPressed); break;
    }
}
int main() {
    // Create instance
    chip8 myChip8;

    //  Load ROM 

    //Emulation Loop
    while (true) {
        myChip8.emulateCycle();
    }

    return 0;
}