#include "AdditiveSynthesizer.h"
#include "../interface/LiveController.h"
#include <iostream>
#include <algorithm>

AdditiveSynthesizer::AdditiveSynthesizer(double sampleRate)
    : Oscillator(sampleRate), amplitude(1.0) {}

void AdditiveSynthesizer::addOscillator(std::unique_ptr<Oscillator> osc) {
    if (osc) {
        osc->setAmplitude(1.0); // Standardize amplitude
        osc->setUsedAsComponent(true);
        oscillators.push_back(std::move(osc));
    }
}

void AdditiveSynthesizer::clearOscillators() {
    oscillators.clear();
}

size_t AdditiveSynthesizer::getOscillatorCount() const {
    return oscillators.size();
}

double AdditiveSynthesizer::nextSample() {
    if (oscillators.empty()) return 0.0;

    double sum = 0.0;
    for (const auto& osc : oscillators) {
        sum += osc->nextSample();
    }
    // Normalize by number of oscillators to avoid clipping
    double normalized = sum / std::max<size_t>(1, oscillators.size());
    return normalized * amplitude;
}

void AdditiveSynthesizer::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
}

void AdditiveSynthesizer::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering additive synth parameters..." << std::endl;

    // Register amplitude parameter for output normalization
    addParameterWithPrefix(controller, prefix, "Amplitude", &amplitude,
                          0.0, 1.0, 1.0,
                          [this]() {
                              amplitude = std::clamp(amplitude, 0.0, 1.0);
                          });

    // Register parameters for each oscillator
    for (size_t i = 0; i < oscillators.size(); ++i) {
        std::string oscPrefix = prefix + " Osc " + std::to_string(i + 1);
        oscillators[i]->registerParametersWithPrefix(controller, oscPrefix);
    }
}
