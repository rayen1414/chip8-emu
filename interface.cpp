#include "interface.hpp"
#include <QPainter>
#include <QPixmap> 
#include<iostream>
#include <QKeyEvent>

Interface::Interface(chip8* emulator, QWidget *parent)
    : QWidget(parent), c8(emulator), buffer(640, 320) { 
    setWindowTitle("chip8");
    setFixedSize(640, 320);
    setFocusPolicy(Qt::StrongFocus);
    buffer.fill(Qt::black);
}

void Interface::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter bufferPainter(&buffer);
    bufferPainter.fillRect(buffer.rect(), Qt::black);
    bufferPainter.setPen(Qt::NoPen);
    bufferPainter.setBrush(Qt::white);

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (c8->gfx[(y * 64) + x] == 1) {
                bufferPainter.drawRect(x * 10, y * 10, 10, 10);
            }
        }
    }
    bufferPainter.end();

    QPainter screenPainter(this);
    screenPainter.drawPixmap(0, 0, buffer); 
}
void Interface::processInput(chip8& c8, int keyboardKey, bool isPressed) {
    switch (keyboardKey) {
        case Qt::Key_1: c8.setKeyState(0x1, isPressed); break;
        case Qt::Key_2: c8.setKeyState(0x2, isPressed); break;
        case Qt::Key_3: c8.setKeyState(0x3, isPressed); break;
        case Qt::Key_4: c8.setKeyState(0xC, isPressed); break;

        case Qt::Key_Q: c8.setKeyState(0x4, isPressed); break;
        case Qt::Key_W: c8.setKeyState(0x5, isPressed); break;
        case Qt::Key_E: c8.setKeyState(0x6, isPressed); break;
        case Qt::Key_R: c8.setKeyState(0xD, isPressed); break;

        case Qt::Key_A: c8.setKeyState(0x7, isPressed); break;
        case Qt::Key_S: c8.setKeyState(0x8, isPressed); break;
        case Qt::Key_D: c8.setKeyState(0x9, isPressed); break;
        case Qt::Key_F: c8.setKeyState(0xE, isPressed); break;

        case Qt::Key_Z: c8.setKeyState(0xA, isPressed); break;
        case Qt::Key_X: c8.setKeyState(0x0, isPressed); break;
        case Qt::Key_C: c8.setKeyState(0xB, isPressed); break;
        case Qt::Key_V: c8.setKeyState(0xF, isPressed); break;
    }
}

void Interface::keyPressEvent(QKeyEvent *event) {
    if (!event->isAutoRepeat() && c8) {
        processInput(*c8, event->key(), true);
    }
}

void Interface::keyReleaseEvent(QKeyEvent *event) {
    if (!event->isAutoRepeat() && c8) {
        processInput(*c8, event->key(), false);
    }
}