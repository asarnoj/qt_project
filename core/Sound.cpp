#include "Sound.h"
#include "../oscillators/SineOscillator.h"  // Fixed: Correct path from core/ to oscillators/
#include <algorithm>

Sound::Sound(double sampleRate)
    : sampleRate(sampleRate), masterVolume(0.7) {
    // Add a default sine oscillator for testing
    auto sineOsc = std::make_unique<SineOscillator>(this->sampleRate);
    sineOsc->setFrequency(440.0);
    sineOsc->setAmplitude(0.5);
    addOscillator(std::move(sineOsc));
}

void Sound::addOscillator(std::unique_ptr<Oscillator> oscillator) {
    oscillators.push_back(std::move(oscillator));
}

void Sound::clearOscillators() {
    oscillators.clear();
}

void Sound::generateSamples(double* buffer, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        buffer[i] = nextSample();
    }
}

double Sound::nextSample() {
    double mixedSample = 0.0;
    
    // Mix all oscillators
    for (auto& osc : oscillators) {
        mixedSample += osc->nextSample();
    }
    
    // Apply master volume and ensure amplitude safety
    mixedSample *= masterVolume;
    return clamp(mixedSample);
}

void Sound::setMasterVolume(double volume) {
    masterVolume = clamp(volume, 0.0, 1.0);
}

double Sound::getMasterVolume() const {
    return masterVolume;
}

double Sound::clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

double Sound::getSampleRate() const {
    return sampleRate;
}
