#include "SineOscillator.h"
#include "../interface/LiveController.h"
#include <iostream>
#include <cmath>

SineOscillator::SineOscillator(double sampleRate) : Oscillator(sampleRate) {
    // Always use standardized amplitude of 1.0
    amplitude = 1.0;
}

double SineOscillator::nextSample() {
    double sample = amplitude * sin(2.0 * M_PI * phase);

    // Advance phase - use custom increment if provided (for FM synthesis)
    if (useCustomPhaseIncrement) {
        phase += customPhaseIncrement;
    } else {
        phase += frequency / sampleRate;
    }
    
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
                          1, 2000, 20,
                          []() { 
                              // No additional update needed
                          });
    
    // No amplitude parameter needed - it's now standardized to 1.0
}