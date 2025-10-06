#include <iostream>
#include <QApplication>
#include "gui/SynthesizerWindow.h"

int main(int argc, char *argv[]) {
    std::cout << "=== Modular Sound Synthesis System ===" << std::endl;
    std::cout << "🎛️ Starting synthesizer application..." << std::endl;
    
    QApplication app(argc, argv);
    
    SynthesizerWindow window;
    window.show();
    
    std::cout << "✨ Application ready!" << std::endl;
    
    return app.exec();
}