#include "SimpleInterface.h"
#include <iostream>
#include <sstream>

SimpleInterface::SimpleInterface(LiveController* controller) 
    : liveController(controller) {
}

void SimpleInterface::showParameters() {
    if (!liveController) return;
    
    std::cout << "\n📋 CURRENT PARAMETERS:" << std::endl;
    std::cout << "=====================" << std::endl;
    
    for (int i = 0; i < liveController->getParameterCount(); ++i) {
        const auto& param = liveController->getParameter(i);
        std::cout << "[" << i << "] " << param.name << ": " 
                  << *param.valuePtr 
                  << " (range: " << param.minValue << "-" << param.maxValue << ")" 
                  << std::endl;
    }
    std::cout << "=====================" << std::endl;
}

void SimpleInterface::showMenu() {
    std::cout << "\n🎛️  SIMPLE CONTROL INTERFACE" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  p        - Show parameters" << std::endl;
    std::cout << "  + <n>    - Increase parameter n" << std::endl;
    std::cout << "  - <n>    - Decrease parameter n" << std::endl;
    std::cout << "  set <n> <value> - Set parameter n to value" << std::endl;
    std::cout << "  q        - Quit" << std::endl;
    std::cout << "Example: '+ 0' increases parameter 0" << std::endl;
}

bool SimpleInterface::handleCommand(const std::string& command) {
    if (!liveController) return false;
    
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "q" || cmd == "quit") {
        return false; // Signal to quit
    } else if (cmd == "p" || cmd == "params") {
        showParameters();
    } else if (cmd == "+" || cmd == "inc") {
        int paramIndex;
        if (iss >> paramIndex) {
            liveController->increaseParameter(paramIndex);
        } else {
            std::cout << "Usage: + <parameter_number>" << std::endl;
        }
    } else if (cmd == "-" || cmd == "dec") {
        int paramIndex;
        if (iss >> paramIndex) {
            liveController->decreaseParameter(paramIndex);
        } else {
            std::cout << "Usage: - <parameter_number>" << std::endl;
        }
    } else if (cmd == "set") {
        int paramIndex;
        double value;
        if (iss >> paramIndex >> value) {
            liveController->setParameter(paramIndex, value);
        } else {
            std::cout << "Usage: set <parameter_number> <value>" << std::endl;
        }
    } else if (cmd == "help" || cmd == "h") {
        showMenu();
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;
    }
    
    return true; // Continue running
}
