#include "LowPassFilter.h"
#include "../interface/LiveController.h"
#include <iostream>
#include <cmath>
#include <algorithm>

// Constructor
LowPassFilter::LowPassFilter(double sampleRate)
    : Filter(sampleRate), cutoffFrequency(1000.0), resonance(0.7071),
      b0(0.0), b1(0.0), b2(0.0), a1(0.0), a2(0.0),
      x1(0.0), x2(0.0), y1(0.0), y2(0.0)
{
    updateCoefficients();
}

double LowPassFilter::processSample(double input) {
    double output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return output;
}

void LowPassFilter::setCutoffFrequency(double freq) {
    cutoffFrequency = std::max(1.0, std::min(freq, sampleRate * 0.45));
    updateCoefficients();
}

void LowPassFilter::setResonance(double q) {
    resonance = std::max(0.1, std::min(q, 10.0));
    updateCoefficients();
}

void LowPassFilter::updateCoefficients() {
    // Standard biquad lowpass (RBJ cookbook) with resonance (Q)
    double omega = 2.0 * M_PI * cutoffFrequency / sampleRate;
    double cosOmega = cos(omega);
    double sinOmega = sin(omega);
    double Q = resonance;
    double alpha = sinOmega / (2.0 * Q);

    double norm = 1.0 + alpha;
    b0 = (1.0 - cosOmega) / 2.0 / norm;
    b1 = (1.0 - cosOmega) / norm;
    b2 = (1.0 - cosOmega) / 2.0 / norm;
    a1 = -2.0 * cosOmega / norm;
    a2 = (1.0 - alpha) / norm;
}

void LowPassFilter::reset() {
    x1 = x2 = y1 = y2 = 0.0;
}

void LowPassFilter::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
}

void LowPassFilter::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering filter parameters..." << std::endl;
    addParameterWithPrefix(controller, prefix, "Cutoff Freq", &cutoffFrequency,
                          20.0, 8000.0, 1000.0,
                          [this]() {
                              setCutoffFrequency(cutoffFrequency);
                          });
    addParameterWithPrefix(controller, prefix, "Resonance", &resonance,
                          0.1, 10.0, 0.7071,
                          [this]() {
                              setResonance(resonance);
                          });
}