#include "SineOscillator.h"
#include "../interface/LiveController.h"
#include <iostream>  // Added missing include
#include <cmath>

SineOscillator::SineOscillator(double sampleRate) : Oscillator(sampleRate) {
    // Always use standardized amplitude of 1.0
    amplitude = 1.0;
}

double SineOscillator::nextSample() {
    double sample = amplitude * sin(2.0 * M_PI * phase);

    // Advance phase
    phase += frequency / sampleRate;
    if (phase >= 1.0)
        phase -= 1.0;

    return sample;
}

void SineOscillator::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
}

void SineOscillator::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering sine parameters..." << std::endl;
    
    // Always register the frequency parameter, even when used as a component
    // This ensures each oscillator defines its own parameter ranges
    addParameterWithPrefix(controller, prefix, "Frequency", &frequency, 
                          1.0, 2000.0, 20.0,
                          [this]() { 
                              // No additional update needed
                          });
    
    // No amplitude parameter needed - it's now standardized to 1.0
}