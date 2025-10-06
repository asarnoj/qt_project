#include "FMSynthesizer.h"
#include "../oscillators/SineOscillator.h"
#include "../interface/LiveController.h"
#include <iostream>
#include <cmath>

FMSynthesizer::FMSynthesizer(double sampleRate)
    : Oscillator(sampleRate), modulationDepth(100.0), 
      carrierFreq(440.0), modulatorFreq(880.0) {
    
    // Standardized amplitude
    amplitude = 1.0;
    carrier = nullptr;
    modulator = nullptr;
    
    // Important: Frequency is no longer used directly by FM synthesizer
    // It just forwards to the carrier oscillator
}

void FMSynthesizer::setCarrierOscillator(std::unique_ptr<Oscillator> carrierOsc) {
    carrier = std::move(carrierOsc);
    if (carrier) {
        carrier->setFrequency(carrierFreq);
        carrier->setAmplitude(1.0);
        carrier->setUsedAsComponent(true);
    }
}

void FMSynthesizer::setModulatorOscillator(std::unique_ptr<Oscillator> modulatorOsc) {
    modulator = std::move(modulatorOsc);
    if (modulator) {
        modulator->setFrequency(modulatorFreq);
        modulator->setAmplitude(1.0);
        modulator->setUsedAsComponent(true);
    }
}

void FMSynthesizer::ensureOscillatorsExist() {
    if (!carrier) {
        carrier = std::make_unique<SineOscillator>(sampleRate);
        carrier->setFrequency(carrierFreq);
        carrier->setAmplitude(1.0);
        carrier->setUsedAsComponent(true);
    }
    
    if (!modulator) {
        modulator = std::make_unique<SineOscillator>(sampleRate);
        modulator->setFrequency(modulatorFreq);
        modulator->setAmplitude(1.0);
        modulator->setUsedAsComponent(true);
    }
}

double FMSynthesizer::nextSample() {
    ensureOscillatorsExist();
    
    // Get the modulator's output sample
    double modulatorOutput = modulator->nextSample();
    
    // IMPORTANT: Use the ACTUAL carrier frequency, not just our stored value
    // This ensures changes to the carrier oscillator are reflected in modulation
    double baseCarrierFreq = carrier->getFrequency();
    
    // Calculate the instantaneous frequency with FM applied
    double modulatedFreq = baseCarrierFreq + (modulatorOutput * modulationDepth);
    
    // Temporarily set the carrier to the modulated frequency
    carrier->setFrequency(modulatedFreq);
    
    // Get the carrier's output sample
    double sample = carrier->nextSample() * amplitude;
    
    // Restore the carrier's original frequency
    carrier->setFrequency(baseCarrierFreq);
    
    return sample;
}

void FMSynthesizer::setFrequency(double freq) {
    // Just update carrier frequency - FM doesn't use frequency directly
    setCarrierFrequency(freq);
}

void FMSynthesizer::setCarrierFrequency(double freq) {
    // Update our internal tracking value
    carrierFreq = freq;
    
    // Update the actual carrier oscillator
    if (carrier) {
        carrier->setFrequency(freq);
    }
    
    // Don't update our own frequency - we're just a container
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

void FMSynthesizer::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering parameters..." << std::endl;
    
    ensureOscillatorsExist();
    
    // Only register modulation depth - this is the FM synthesizer's only real parameter
    addParameterWithPrefix(controller, prefix, "Mod Depth", &modulationDepth, 
                          0.0, 1000.0, 20.0,
                          [this]() {
                              if (modulationDepth < 0.0) modulationDepth = 0.0;
                          });
    
    // Register carrier oscillator's parameters with proper prefix
    if (carrier) {
        std::string carrierPrefix = prefix + " Carrier";
        if (auto* fmCarrier = dynamic_cast<FMSynthesizer*>(carrier.get())) {
            std::cout << "🔍 Carrier is nested FM synthesizer - registering its parameters..." << std::endl;
            fmCarrier->registerParametersWithPrefix(controller, carrierPrefix);
        } else {
            std::cout << "🔍 Registering carrier oscillator parameters..." << std::endl;
            carrier->registerParametersWithPrefix(controller, carrierPrefix);
        }
    }
    
    // Register modulator oscillator's parameters with proper prefix
    if (modulator) {
        std::string modPrefix = prefix + " Modulator";
        if (auto* fmModulator = dynamic_cast<FMSynthesizer*>(modulator.get())) {
            std::cout << "🔍 Modulator is nested FM synthesizer - registering its parameters..." << std::endl;
            fmModulator->registerParametersWithPrefix(controller, modPrefix);
        } else {
            std::cout << "🔍 Registering modulator oscillator parameters..." << std::endl;
            modulator->registerParametersWithPrefix(controller, modPrefix);
        }
    }
}

void FMSynthesizer::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
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

