#include "SawOscillator.h"
#include "../interface/LiveController.h"
#include <iostream>
#include <cmath>

SawOscillator::SawOscillator(double sampleRate) : Oscillator(sampleRate) {
    // Always use standardized amplitude of 1.0
    amplitude = 1.0;
}

double SawOscillator::nextSample() {
    // Generate sawtooth wave: linear ramp from -1 to 1
    double sample = amplitude * (2.0 * phase - 1.0);

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

void SawOscillator::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
}

void SawOscillator::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering saw parameters..." << std::endl;
    
    // Always register the frequency parameter, even when used as a component
    // This ensures each oscillator defines its own parameter ranges
    addParameterWithPrefix(controller, prefix, "Frequency", &frequency, 
                          1, 2000, 20,
                          []() { 
                              // No additional update needed
                          });
    
    // No amplitude parameter needed - it's now standardized to 1.0
}
