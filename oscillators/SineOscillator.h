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
    
    // Add phase access and temporary phase increment for FM synthesis
    double getPhase() const { return phase; }
    void setPhaseIncrement(double increment) { 
        customPhaseIncrement = increment; 
        useCustomPhaseIncrement = true; 
    }
    void resetPhaseIncrement() { useCustomPhaseIncrement = false; }

private:
    bool useCustomPhaseIncrement = false;
    double customPhaseIncrement = 0.0;
};

#endif // SINEOSCILLATOR_H