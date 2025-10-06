#include "LiveController.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

LiveController::LiveController() {
}

void LiveController::addParameter(const std::string& name, double* valuePtr, 
                                 double minValue, double maxValue, double step) {
    parameters.emplace_back(name, valuePtr, minValue, maxValue, step);
}

void LiveController::clearParameters() {
    parameters.clear();
}

void LiveController::increaseParameter(int index) {
    if (index >= 0 && index < static_cast<int>(parameters.size())) {
        *parameters[index].valuePtr += parameters[index].step;
        clampValue(index);
        std::cout << "📈 " << parameters[index].name << ": " 
                  << std::fixed << std::setprecision(2) << *parameters[index].valuePtr << std::endl;
    }
}

void LiveController::decreaseParameter(int index) {
    if (index >= 0 && index < static_cast<int>(parameters.size())) {
        *parameters[index].valuePtr -= parameters[index].step;
        clampValue(index);
        std::cout << "📉 " << parameters[index].name << ": " 
                  << std::fixed << std::setprecision(2) << *parameters[index].valuePtr << std::endl;
    }
}

void LiveController::setParameter(int index, double value) {
    if (index >= 0 && index < static_cast<int>(parameters.size())) {
        *parameters[index].valuePtr = value;
        clampValue(index);
        std::cout << "🎛️  " << parameters[index].name << " set to: " 
                  << std::fixed << std::setprecision(2) << *parameters[index].valuePtr << std::endl;
    }
}

void LiveController::clampValue(int index) {
    auto& param = parameters[index];
    *param.valuePtr = std::max(param.minValue, std::min(param.maxValue, *param.valuePtr));
}

void LiveController::printParameters() const {
    std::cout << "\n🎛️  LIVE PARAMETERS" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    for (int i = 0; i < static_cast<int>(parameters.size()); ++i) {
        const auto& param = parameters[i];
        std::cout << "[" << i << "] " << std::setw(15) << param.name << ": " 
                  << std::setw(8) << std::fixed << std::setprecision(2) << *param.valuePtr
                  << " (" << param.minValue << " - " << param.maxValue << ")" << std::endl;
    }
    std::cout << std::string(50, '=') << std::endl;
}

void LiveController::printControls() const {
    std::cout << "\n🎮 CONTROLS:" << std::endl;
    std::cout << "  Numbers 0-9: Select parameter" << std::endl;
    std::cout << "  +/-: Increase/decrease selected parameter" << std::endl;
    std::cout << "  p: Print current parameters" << std::endl;
    std::cout << "  q: Quit" << std::endl;
}
