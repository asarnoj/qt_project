#include "Sound.h"
#include "../oscillators/SineOscillator.h"  // Fixed: Correct path from core/ to oscillators/
#include "Filter.h"
#include <algorithm>

Sound::Sound(double sampleRate) : sampleRate(sampleRate), masterVolume(0.7) {
}

void Sound::addOscillator(std::unique_ptr<Oscillator> oscillator) {
    oscillators.push_back(std::move(oscillator));
    mixRatios.push_back(1.0);  // Default equal mix
    normalizeMixRatios();
}

void Sound::addFilter(std::unique_ptr<Filter> filter) {
    filters.push_back(std::move(filter));
}

void Sound::addEnvelope(std::unique_ptr<Envelope> env) {
    envelopes.push_back(std::move(env));
}

void Sound::clearFilters() {
    filters.clear();
}

void Sound::clearOscillators() {
    oscillators.clear();
    mixRatios.clear();
    clearFilters();  // Also clear filters when clearing oscillators
}

void Sound::clearEnvelopes() {
    envelopes.clear();
}

void Sound::generateSamples(double* buffer, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        buffer[i] = nextSample();
    }
}

double Sound::nextSample() {
    double sample = 0.0;
    double oscSum = 0.0;
    // Mix all oscillators with their ratios (each oscillator outputs at amplitude 1.0)
    for (size_t i = 0; i < oscillators.size(); ++i) {
        double oscSample = oscillators[i]->nextSample();
        oscSum += oscSample * mixRatios[i];
    }
    sample = oscSum;
    
    // Process through all filters in sequence
    for (auto& filter : filters) {
        sample = filter->processSample(sample);
    }
    
    double env = 1.0;
    // Apply envelope if available
    if (!envelopes.empty() && envelopes[0]) {
        env = envelopes[0]->nextSample();
        sample *= env;
        // No need to forcibly mute here; envelope will return 0.0 when inactive
    }

    // Apply master volume
    sample *= masterVolume;

    // Debug print
    // std::cout << "oscSum: " << oscSum << " env: " << env << " sample: " << sample << std::endl;

    return sample;
}

double Sound::clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
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

double Sound::getSampleRate() const {
    return sampleRate;
}

Envelope* Sound::getEnvelope(int idx) {
    if (idx >= 0 && idx < static_cast<int>(envelopes.size()))
        return envelopes[idx].get();
    return nullptr;
}

void Sound::noteOn() {
    for (auto& env : envelopes) {
        env->noteOn();
    }
}

void Sound::noteOff() {
    for (auto& env : envelopes) {
        env->noteOff();
    }
}