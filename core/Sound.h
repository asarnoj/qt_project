#ifndef SOUND_H
#define SOUND_H

#include <vector>
#include <memory>
#include "Oscillator.h"

class Sound {
public:
    Sound(double sampleRate = 44100.0);
    ~Sound() = default;

    // Module management
    void addOscillator(std::unique_ptr<Oscillator> oscillator);
    void clearOscillators();
    
    // Audio generation
    void generateSamples(double* buffer, int numSamples);
    double nextSample();
    
    // Master volume control
    void setMasterVolume(double volume);
    void updateMasterVolume() { setMasterVolume(*getMasterVolumePtr()); } // New method
    double getMasterVolume() const { return masterVolume; }
    double* getMasterVolumePtr() { return &masterVolume; }  // Add this method
    
    // Mix ratios for combining oscillators
    void setOscillatorMixRatio(int index, double ratio);
    double getOscillatorMixRatio(int index) const;
    
    int getOscillatorCount() const { return static_cast<int>(oscillators.size()); }
    
    // Add missing method declarations
    double getSampleRate() const;
    double clamp(double value, double min, double max);

private:
    std::vector<std::unique_ptr<Oscillator>> oscillators;
    std::vector<double> mixRatios;  // Mix ratios for each oscillator
    double sampleRate;
    double masterVolume;
    
    void normalizeMixRatios();  // Ensure ratios sum to 1.0
};

#endif // SOUND_H
