#include "FMSynthesizer.h"
#include "../oscillators/SineOscillator.h"
#include "../interface/LiveController.h"
#include <iostream>  // Added missing include for std::cout
#include <cmath>

FMSynthesizer::FMSynthesizer(double sampleRate)
    : Oscillator(sampleRate), modulationDepth(100.0), 
      carrierFreq(440.0), modulatorFreq(880.0) {
    
    // Start with no oscillators - they can be injected or will be created on demand
    carrier = nullptr;
    modulator = nullptr;
}

void FMSynthesizer::setCarrierOscillator(std::unique_ptr<Oscillator> carrierOsc) {
    carrier = std::move(carrierOsc);
    if (carrier) {
        carrier->setFrequency(carrierFreq);
        carrier->setAmplitude(1.0);
        // Mark as component so it doesn't register its own parameters
        carrier->setUsedAsComponent(true);
    }
}

void FMSynthesizer::setModulatorOscillator(std::unique_ptr<Oscillator> modulatorOsc) {
    modulator = std::move(modulatorOsc);
    if (modulator) {
        modulator->setFrequency(modulatorFreq);
        modulator->setAmplitude(1.0);
        // Mark as component so it doesn't register its own parameters
        modulator->setUsedAsComponent(true);
    }
}

void FMSynthesizer::ensureOscillatorsExist() {
    // Create default sine oscillators if none provided
    if (!carrier) {
        carrier = std::make_unique<SineOscillator>(sampleRate);
        carrier->setFrequency(carrierFreq);
        carrier->setAmplitude(1.0);
        // Mark as component
        carrier->setUsedAsComponent(true);
    }
    
    if (!modulator) {
        modulator = std::make_unique<SineOscillator>(sampleRate);
        modulator->setFrequency(modulatorFreq);
        modulator->setAmplitude(1.0);
        // Mark as component
        modulator->setUsedAsComponent(true);
    }
}

double FMSynthesizer::nextSample() {
    ensureOscillatorsExist();
    
    // Get modulator output (can be from ANY oscillator type!)
    double modulatorOutput = modulator->nextSample();
    
    // Apply modulation to carrier frequency
    double originalCarrierFreq = carrier->getFrequency();
    double modulatedFreq = carrierFreq + (modulatorOutput * modulationDepth);
    
    // Temporarily set carrier frequency for this sample
    carrier->setFrequency(modulatedFreq);
    
    // Get carrier output (can be from ANY oscillator type!)
    double sample = carrier->nextSample() * amplitude;
    
    // Restore original carrier frequency
    carrier->setFrequency(originalCarrierFreq);
    
    return sample;
}

void FMSynthesizer::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
}

void FMSynthesizer::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering parameters..." << std::endl;
    
    ensureOscillatorsExist();
    
    // Register our own FM-specific parameters (these control the carrier/modulator indirectly)
    addParameterWithPrefix(controller, prefix, "Carrier Freq", &carrierFreq, 200.0, 800.0, 10.0,
                          [this]() { 
                              if (carrier) carrier->setFrequency(carrierFreq);
                              frequency = carrierFreq;
                          });
    
    addParameterWithPrefix(controller, prefix, "Mod Freq", &modulatorFreq, 100.0, 2000.0, 50.0,
                          [this]() { 
                              if (modulator) modulator->setFrequency(modulatorFreq);
                          });
    
    addParameterWithPrefix(controller, prefix, "Mod Depth", &modulationDepth, 0.0, 500.0, 10.0,
                          [this]() {
                              if (modulationDepth < 0.0) modulationDepth = 0.0;
                          });
    
    addParameterWithPrefix(controller, prefix, "Amplitude", &amplitude, 0.0, 0.8, 0.02,
                          [this]() {
                              if (amplitude > 0.8) amplitude = 0.8;
                              if (amplitude < 0.0) amplitude = 0.0;
                          });
    
    // RECURSIVE: Only register parameters from nested oscillators if they are NOT marked as components
    if (carrier && !carrier->getIsUsedAsComponent()) {
        std::cout << "🔍 Discovering parameters from standalone carrier oscillator..." << std::endl;
        carrier->registerParametersWithPrefix(controller, prefix + " Carrier");
    } else if (carrier) {
        std::cout << "🔧 Carrier is FM-controlled component, skipping individual parameters" << std::endl;
        
        // But if it's another FM synthesizer, we DO want its parameters
        if (auto* fmCarrier = dynamic_cast<FMSynthesizer*>(carrier.get())) {
            std::cout << "🔍 Carrier is nested FM synthesizer - registering its parameters..." << std::endl;
            fmCarrier->registerParametersWithPrefix(controller, prefix + " Carrier");
        }
    }
    
    if (modulator && !modulator->getIsUsedAsComponent()) {
        std::cout << "🔍 Discovering parameters from standalone modulator oscillator..." << std::endl;
        modulator->registerParametersWithPrefix(controller, prefix + " Modulator");
    } else if (modulator) {
        std::cout << "🔧 Modulator is FM-controlled component, skipping individual parameters" << std::endl;
        
        // But if it's another FM synthesizer, we DO want its parameters
        if (auto* fmModulator = dynamic_cast<FMSynthesizer*>(modulator.get())) {
            std::cout << "🔍 Modulator is nested FM synthesizer - registering its parameters..." << std::endl;
            fmModulator->registerParametersWithPrefix(controller, prefix + " Modulator");
        }
    }
}

void FMSynthesizer::setFrequency(double freq) {
    // Override base class to set carrier frequency
    frequency = freq;  // Update base class frequency
    carrierFreq = freq;
    if (carrier) {
        carrier->setFrequency(freq);
    }
}

void FMSynthesizer::setCarrierFrequency(double freq) {
    carrierFreq = freq;
    if (carrier) {
        carrier->setFrequency(freq);
    }
    frequency = freq;  // Keep base class in sync
}

void FMSynthesizer::setModulatorFrequency(double modFreq) {
    modulatorFreq = modFreq;
    if (modulator) {
        modulator->setFrequency(modFreq);
    }
}

void FMSynthesizer::setModulationDepth(double depth) {
    modulationDepth = (depth < 0.0) ? 0.0 : depth;
}

void FMSynthesizer::setModulatorAmplitude(double amp) {
    if (modulator) {
        modulator->setAmplitude(amp);
    }
}

double FMSynthesizer::getCarrierFrequency() const {
    return carrierFreq;
}

double FMSynthesizer::getModulatorFrequency() const {
    return modulatorFreq;
}

double FMSynthesizer::getModulationDepth() const {
    return modulationDepth;
}

double FMSynthesizer::getModulatorAmplitude() const {
    return modulator ? modulator->getAmplitude() : 0.0;
}
