#ifndef FMSYNTHESIZER_H
#define FMSYNTHESIZER_H

#include "../core/Oscillator.h"
#include <memory>

class FMSynthesizer : public Oscillator {
public:
    FMSynthesizer(double sampleRate = 44100.0);
    double nextSample() override;
    
    // Modular oscillator injection - accept ANY oscillator type as carrier/modulator
    void setCarrierOscillator(std::unique_ptr<Oscillator> carrierOsc);
    void setModulatorOscillator(std::unique_ptr<Oscillator> modulatorOsc);
    
    // FM-specific parameters - now primarily managing carrier and modulator
    void setCarrierFrequency(double freq);
    void setModulatorFrequency(double modFreq);
    void setModulationDepth(double depth);
    void setModulatorAmplitude(double amp);
    
    // Getters
    double getCarrierFrequency() const;
    double getModulatorFrequency() const;
    double getModulationDepth() const;
    double getModulatorAmplitude() const;
    
    // Override base setters to affect carrier
    void setFrequency(double freq) override; // Now just passes through to carrier
    
    // Automatic parameter registration
    void registerParameters(LiveController& controller) override;
    void registerParametersWithPrefix(LiveController& controller, const std::string& prefix) override;
    std::string getTypeName() const override { return "FM"; }

private:
    std::unique_ptr<Oscillator> carrier;      // ANY oscillator type
    std::unique_ptr<Oscillator> modulator;    // ANY oscillator type
    double modulationDepth;
    double carrierFreq;    // Only stored for parameter initialization
    double modulatorFreq;  // Only stored for parameter initialization
    
    // Helper to create default oscillators if none provided
    void ensureOscillatorsExist();
};

#endif // FMSYNTHESIZER_H
