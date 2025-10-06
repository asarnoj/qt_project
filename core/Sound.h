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
    
    // Global parameters
    void setMasterVolume(double volume);
    double getMasterVolume() const;
    double getSampleRate() const;
    
    // Safety
    static double clamp(double value, double min = -1.0, double max = 1.0);

private:
    std::vector<std::unique_ptr<Oscillator>> oscillators;
    double sampleRate;
    double masterVolume;
};

#endif // SOUND_H
