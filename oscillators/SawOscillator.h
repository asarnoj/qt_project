#ifndef SAWOSCILLATOR_H
#define SAWOSCILLATOR_H

#include "../core/Oscillator.h"

class SawOscillator : public Oscillator {
public:
    SawOscillator(double sampleRate = 44100.0);
    double nextSample() override;
    
    // Automatic parameter registration
    void registerParameters(LiveController& controller) override;
    void registerParametersWithPrefix(LiveController& controller, const std::string& prefix) override;
    std::string getTypeName() const override { return "Saw"; }
    
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

#endif // SAWOSCILLATOR_H
