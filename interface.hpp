#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include <QWidget>
#include "chip8.hpp"
#include <QPixmap> 

class Interface : public QWidget {
    Q_OBJECT

public:
    Interface(chip8* emulator, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void processInput(chip8& c8, int keyboardKey, bool isPressed);

    chip8* c8;
    QPixmap buffer;    
};

#endif