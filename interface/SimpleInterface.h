#ifndef SIMPLEINTERFACE_H
#define SIMPLEINTERFACE_H

#include "LiveController.h"
#include <string>

class SimpleInterface {
public:
    SimpleInterface(LiveController* controller);
    
    // Very basic interface functions
    void showParameters();
    void showMenu();
    bool handleCommand(const std::string& command);
    
private:
    LiveController* liveController;
};

#endif // SIMPLEINTERFACE_H
