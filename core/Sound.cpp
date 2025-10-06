#include "Sound.h"
#include "../oscillators/SineOscillator.h"  // Fixed: Correct path from core/ to oscillators/
#include <algorithm>

Sound::Sound(double sampleRate) : sampleRate(sampleRate), masterVolume(0.7) {
}

void Sound::addOscillator(std::unique_ptr<Oscillator> oscillator) {
    oscillators.push_back(std::move(oscillator));
    mixRatios.push_back(1.0);  // Default equal mix
    normalizeMixRatios();
}

void Sound::clearOscillators() {
    oscillators.clear();
    mixRatios.clear();
}

void Sound::generateSamples(double* buffer, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        buffer[i] = nextSample();
    }
}

double Sound::nextSample() {
    if (oscillators.empty()) {
        return 0.0;
    }
    
    double mixedSample = 0.0;
    
    // Mix all oscillators with their ratios (each oscillator outputs at amplitude 1.0)
    for (size_t i = 0; i < oscillators.size(); ++i) {
        double sample = oscillators[i]->nextSample();
        mixedSample += sample * mixRatios[i];
    }
    
    // Apply master volume
    return mixedSample * masterVolume;
}

void Sound::setMasterVolume(double volume) {
    masterVolume = std::max(0.0, std::min(1.0, volume));
}

void Sound::setOscillatorMixRatio(int index, double ratio) {
    if (index >= 0 && index < static_cast<int>(mixRatios.size())) {
        mixRatios[index] = std::max(0.0, std::min(1.0, ratio));
        normalizeMixRatios();
    }
}

double Sound::getOscillatorMixRatio(int index) const {
    if (index >= 0 && index < static_cast<int>(mixRatios.size())) {
        return mixRatios[index];
    }
    return 0.0;
}

void Sound::normalizeMixRatios() {
    if (mixRatios.empty()) return;
    
    // Calculate sum
    double sum = 0.0;
    for (double ratio : mixRatios) {
        sum += ratio;
    }
    
    // Normalize so they sum to 1.0
    if (sum > 0.0) {
        for (double& ratio : mixRatios) {
            ratio /= sum;
        }
    } else {
        // If all ratios are 0, set equal ratios
        double equalRatio = 1.0 / mixRatios.size();
        for (double& ratio : mixRatios) {
            ratio = equalRatio;
        }
    }
}

double Sound::clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

double Sound::getSampleRate() const {
    return sampleRate;
}
