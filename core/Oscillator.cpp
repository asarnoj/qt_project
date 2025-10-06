#include "Oscillator.h"
#include "../interface/LiveController.h"

Oscillator::Oscillator(double sampleRate)
    : frequency(440.0), amplitude(0.5), sampleRate(sampleRate), phase(0.0) {}

void Oscillator::setFrequency(double freq) {
    frequency = freq;
}

void Oscillator::setAmplitude(double amp) {
    amplitude = (amp < 0.0) ? 0.0 : (amp > 1.0 ? 1.0 : amp);
}

void Oscillator::setSampleRate(double rate) {
    sampleRate = rate;
}

double Oscillator::getFrequency() const {
    return frequency;
}

double Oscillator::getAmplitude() const {
    return amplitude;
}

double Oscillator::getSampleRate() const {
    return sampleRate;
}

void Oscillator::addParameterWithPrefix(LiveController& controller, const std::string& prefix,
                                       const std::string& name, double* valuePtr, double minVal, 
                                       double maxVal, double step, std::function<void()> /* callback */) {
    // Create full parameter name with prefix
    std::string fullName = prefix + " " + name;
    controller.addParameter(fullName, valuePtr, minVal, maxVal, step);
}

void Oscillator::addParameter(LiveController& controller, const std::string& name, 
                             double* valuePtr, double minVal, double maxVal, double step,
                             std::function<void()> /* callback */) {
    // Create parameter name with oscillator type prefix
    std::string fullName = getTypeName() + " " + name;
    controller.addParameter(fullName, valuePtr, minVal, maxVal, step);
}