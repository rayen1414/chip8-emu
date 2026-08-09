#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include<QWidget>
#include "chip8.hpp"
#include <QPaintEvent>
class Interface : public QWidget{
    Q_OBJECT
    private:
    chip8* c8;
    void paintEvent(QPaintEvent *event);
    void processInput(chip8& c8, int keyboardKey, bool isPressed);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    public:
    explicit Interface(chip8* emulator, QWidget *parent = nullptr);
    ~Interface() override = default;

};

#endif