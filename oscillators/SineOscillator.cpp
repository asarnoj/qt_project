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
    
    // Only register frequency parameter - amplitude is always 1.0
    addParameterWithPrefix(controller, prefix, "Frequency", &frequency, 100.0, 1000.0, 20.0,
                          []() { 
                              // No additional update needed
                          });
    
    // Remove amplitude parameter registration - it's now standardized to 1.0
}