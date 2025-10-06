#ifndef SINEOSCILLATOR_H
#define SINEOSCILLATOR_H

#include "../core/Oscillator.h"

class SineOscillator : public Oscillator {
public:
    SineOscillator(double sampleRate = 44100.0);
    double nextSample() override;
    
    // Automatic parameter registration
    void registerParameters(LiveController& controller) override;
    void registerParametersWithPrefix(LiveController& controller, const std::string& prefix) override;
    std::string getTypeName() const override { return "Sine"; }
};

#endif // SINEOSCILLATOR_H