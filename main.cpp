#include <QApplication>
#include <iostream>
#include "chip8.hpp"
#include "interface.hpp"

int main(int argc, char *argv[]){
    // Create instance
    chip8 myChip8;
    QApplication app(argc, argv);
    Interface window(&myChip8);
    window.show();
    return app.exec();
    //  Load ROM 
    if (argc > 1) mychip8.loadfile(argv[1]);
    else return(0);
    //Emulation Loop
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        myChip8.emulateCycle(); //run one opcode
        if (myChip8.drawflag) {//if there is pix change we redraw
            window.update();          
            myChip8.drawflag = false; // Reset flag
        }
    });

    timer.start(2); // Runs every 2 milliseconds
    return app.exec();
    return 0;
}