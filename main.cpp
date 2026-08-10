#include <QApplication>
#include <iostream>
#include "chip8.hpp"
#include <QTimer>
#include "interface.hpp"

int main(int argc, char *argv[]){
    srand(static_cast<unsigned>(time(nullptr))); 
    // Create instance
    chip8 myChip8;
    QApplication app(argc, argv);
    Interface window(&myChip8);
    window.show();
    //  Load ROM 
    if (argc > 1) {
    if (!myChip8.loadfile(argv[1])) {
        std::cerr << "Exiting due to failed ROM load." << std::endl;
        return 1; 
    }
}
    else return(1);
    
    //Emulation Loop
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        for (int i = 0; i < 10; ++i) {
            myChip8.emulateCycle();
        }//run 10 opcode
        if (myChip8.delay_timer > 0) --myChip8.delay_timer;
        if (myChip8.sound_timer > 0) --myChip8.sound_timer;
        if (myChip8.drawflag) {
            window.update();          
            myChip8.drawflag = false; // Reset flag
            
        }
    });

    timer.start(16); // Runs every 2 milliseconds
    return app.exec();
    return 0;
}