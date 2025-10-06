#include "PresetLibrary.h"
#include "../oscillators/SineOscillator.h"
#include "../synthesizers/FMSynthesizer.h"
#include <iostream>

void PresetLibrary::registerAllPresets(SoundInterface& interface) {
    // Register Simple Sine
    interface.registerPreset("Simple Sine", [](Sound* sound, SoundInterface* iface) {
        setupSimpleSine(sound, *iface);
    });
    
    // Register Harmony
    interface.registerPreset("Harmony Chord", [](Sound* sound, SoundInterface* iface) {
        setupHarmony(sound, *iface);
    });
    
    // Register FM Exploration  
    interface.registerPreset("FM Bell", [](Sound* sound, SoundInterface* iface) {
        setupFMExploration(sound, *iface);
    });
    
    // Register Layered Texture
    interface.registerPreset("Layered Texture", [](Sound* sound, SoundInterface* iface) {
        setupLayeredTexture(sound, *iface);
    });
}

void PresetLibrary::setupSimpleSine(Sound* sound, SoundInterface& interface) {
    std::cout << "🎵 Setup: Simple 440Hz sine wave" << std::endl;
    sound->clearOscillators();
    
    auto sine = std::make_unique<SineOscillator>(44100.0);
    sine->setFrequency(440.0);
    sine->setAmplitude(0.4);
    
    // Store references for parameter control
    auto* sinePtr = sine.get();
    sound->addOscillator(std::move(sine));
    
    // Register controllable parameters
    interface.addParameter("Frequency", ParameterType::FREQUENCY, 
                          440.0, 80.0, 2000.0, "Hz",
                          [sinePtr](double val) { sinePtr->setFrequency(val); });
                          
    interface.addParameter("Amplitude", ParameterType::AMPLITUDE,
                          0.4, 0.0, 1.0, "",
                          [sinePtr](double val) { sinePtr->setAmplitude(val); });
                          
    interface.addParameter("Master Volume", ParameterType::MASTER_VOLUME,
                          0.7, 0.0, 1.0, "",
                          [sound](double val) { sound->setMasterVolume(val); });
}

void PresetLibrary::setupHarmony(Sound* sound, SoundInterface& interface) {
    std::cout << "🎵 Setup: Harmonic chord (C major: C-E-G)" << std::endl;
    sound->clearOscillators();
    
    // C note
    auto c = std::make_unique<SineOscillator>(44100.0);
    c->setFrequency(261.63);
    c->setAmplitude(0.2);
    auto* cPtr = c.get();
    sound->addOscillator(std::move(c));
    
    // E note
    auto e = std::make_unique<SineOscillator>(44100.0);
    e->setFrequency(329.63);
    e->setAmplitude(0.2);
    auto* ePtr = e.get();
    sound->addOscillator(std::move(e));
    
    // G note
    auto g = std::make_unique<SineOscillator>(44100.0);
    g->setFrequency(392.00);
    g->setAmplitude(0.2);
    auto* gPtr = g.get();
    sound->addOscillator(std::move(g));
    
    // Register parameters for each note
    interface.addParameter("C Note Freq", ParameterType::FREQUENCY,
                          261.63, 200.0, 400.0, "Hz",
                          [cPtr](double val) { cPtr->setFrequency(val); });
    interface.addParameter("E Note Freq", ParameterType::FREQUENCY,
                          329.63, 250.0, 500.0, "Hz",
                          [ePtr](double val) { ePtr->setFrequency(val); });
    interface.addParameter("G Note Freq", ParameterType::FREQUENCY,
                          392.00, 300.0, 600.0, "Hz",
                          [gPtr](double val) { gPtr->setFrequency(val); });
    interface.addParameter("C Amplitude", ParameterType::AMPLITUDE,
                          0.2, 0.0, 0.5, "",
                          [cPtr](double val) { cPtr->setAmplitude(val); });
    interface.addParameter("E Amplitude", ParameterType::AMPLITUDE,
                          0.2, 0.0, 0.5, "",
                          [ePtr](double val) { ePtr->setAmplitude(val); });
    interface.addParameter("G Amplitude", ParameterType::AMPLITUDE,
                          0.2, 0.0, 0.5, "",
                          [gPtr](double val) { gPtr->setAmplitude(val); });
}

void PresetLibrary::setupFMExploration(Sound* sound, SoundInterface& interface) {
    std::cout << "🎵 Setup: FM synthesis exploration" << std::endl;
    sound->clearOscillators();
    
    auto fmBell = std::make_unique<FMSynthesizer>(44100.0);
    fmBell->setCarrierFrequency(440.0);
    fmBell->setModulatorFrequency(880.0);
    fmBell->setModulationDepth(200.0);
    fmBell->setAmplitude(0.3);
    
    auto* fmPtr = fmBell.get();
    sound->addOscillator(std::move(fmBell));
    
    // Register FM-specific parameters
    interface.addParameter("Carrier Freq", ParameterType::FREQUENCY,
                          440.0, 100.0, 1000.0, "Hz",
                          [fmPtr](double val) { fmPtr->setCarrierFrequency(val); });
    interface.addParameter("Modulator Freq", ParameterType::FM_MODULATOR_FREQ,
                          880.0, 50.0, 2000.0, "Hz",
                          [fmPtr](double val) { fmPtr->setModulatorFrequency(val); });
    interface.addParameter("Modulation Depth", ParameterType::FM_MODULATION_DEPTH,
                          200.0, 0.0, 500.0, "",
                          [fmPtr](double val) { fmPtr->setModulationDepth(val); });
    interface.addParameter("FM Amplitude", ParameterType::AMPLITUDE,
                          0.3, 0.0, 0.8, "",
                          [fmPtr](double val) { fmPtr->setAmplitude(val); });
}

void PresetLibrary::setupLayeredTexture(Sound* sound, SoundInterface& interface) {
    std::cout << "🎵 Setup: Layered texture (Bass + FM + Harmony)" << std::endl;
    sound->clearOscillators();
    
    // Bass
    auto bass = std::make_unique<SineOscillator>(44100.0);
    bass->setFrequency(110.0);
    bass->setAmplitude(0.15);
    auto* bassPtr = bass.get();
    sound->addOscillator(std::move(bass));
    
    // FM texture
    auto fmTexture = std::make_unique<FMSynthesizer>(44100.0);
    fmTexture->setCarrierFrequency(440.0);
    fmTexture->setModulatorFrequency(110.0);
    fmTexture->setModulationDepth(50.0);
    fmTexture->setAmplitude(0.1);
    auto* fmPtr = fmTexture.get();
    sound->addOscillator(std::move(fmTexture));
    
    // Harmony
    auto harmony = std::make_unique<SineOscillator>(44100.0);
    harmony->setFrequency(880.0);
    harmony->setAmplitude(0.08);
    auto* harmonyPtr = harmony.get();
    sound->addOscillator(std::move(harmony));
    
    // Register all parameters
    interface.addParameter("Bass Freq", ParameterType::FREQUENCY,
                          110.0, 50.0, 300.0, "Hz",
                          [bassPtr](double val) { bassPtr->setFrequency(val); });
    interface.addParameter("Bass Level", ParameterType::AMPLITUDE,
                          0.15, 0.0, 0.3, "",
                          [bassPtr](double val) { bassPtr->setAmplitude(val); });
    interface.addParameter("FM Carrier", ParameterType::FREQUENCY,
                          440.0, 200.0, 800.0, "Hz",
                          [fmPtr](double val) { fmPtr->setCarrierFrequency(val); });
    interface.addParameter("FM Mod Freq", ParameterType::FM_MODULATOR_FREQ,
                          110.0, 50.0, 500.0, "Hz",
                          [fmPtr](double val) { fmPtr->setModulatorFrequency(val); });
    interface.addParameter("FM Depth", ParameterType::FM_MODULATION_DEPTH,
                          50.0, 0.0, 200.0, "",
                          [fmPtr](double val) { fmPtr->setModulationDepth(val); });
    interface.addParameter("FM Level", ParameterType::AMPLITUDE,
                          0.1, 0.0, 0.3, "",
                          [fmPtr](double val) { fmPtr->setAmplitude(val); });
    interface.addParameter("Harmony Freq", ParameterType::FREQUENCY,
                          880.0, 400.0, 1500.0, "Hz",
                          [harmonyPtr](double val) { harmonyPtr->setFrequency(val); });
    interface.addParameter("Harmony Level", ParameterType::AMPLITUDE,
                          0.08, 0.0, 0.2, "",
                          [harmonyPtr](double val) { harmonyPtr->setAmplitude(val); });
}
