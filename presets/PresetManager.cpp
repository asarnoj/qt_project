#include "PresetManager.h"
#include "../oscillators/SineOscillator.h"
#include "../synthesizers/FMSynthesizer.h"
#include <iostream>

PresetManager::PresetManager() {
    // Register all built-in presets
    registerPreset("Simple Sine Wave", "Single sine oscillator with frequency and amplitude control", setupSimpleSine);
    registerPreset("FM Synthesizer", "Basic FM synthesis with sine carrier and modulator", setupBasicFM);
    registerPreset("Nested FM", "Advanced nested FM synthesis for complex timbres", setupNestedFM);
}

void PresetManager::registerPreset(const std::string& name, const std::string& description, PresetSetupFunction setupFunc) {
    presets.push_back({name, description, setupFunc});
}

void PresetManager::loadPreset(int index, Sound* sound, LiveController& controller) {
    if (index >= 0 && index < static_cast<int>(presets.size())) {
        std::cout << "🎵 Loading preset: " << presets[index].name << std::endl;
        std::cout << "   Description: " << presets[index].description << std::endl;
        
        sound->clearOscillators();
        controller.clearParameters();
        
        presets[index].setupFunction(sound, controller);
        
        std::cout << "✨ Loaded with " << controller.getParameterCount() << " parameters" << std::endl;
    }
}

std::vector<std::string> PresetManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : presets) {
        names.push_back(preset.name);
    }
    return names;
}

// Built-in preset implementations
void PresetManager::setupSimpleSine(Sound* sound, LiveController& controller) {
    auto sine = std::make_unique<SineOscillator>(44100.0);
    sine->setFrequency(440.0);
    sine->setAmplitude(0.4);
    sine->registerParameters(controller);
    sound->addOscillator(std::move(sine));
}

void PresetManager::setupBasicFM(Sound* sound, LiveController& controller) {
    auto fmSynth = std::make_unique<FMSynthesizer>(44100.0);
    
    // Create carrier and modulator
    auto carrier = std::make_unique<SineOscillator>(44100.0);
    carrier->setFrequency(440.0);
    carrier->setAmplitude(1.0);
    
    auto modulator = std::make_unique<SineOscillator>(44100.0);
    modulator->setFrequency(880.0);
    modulator->setAmplitude(1.0);
    
    // Inject oscillators
    fmSynth->setCarrierOscillator(std::move(carrier));
    fmSynth->setModulatorOscillator(std::move(modulator));
    fmSynth->registerParameters(controller);
    
    sound->addOscillator(std::move(fmSynth));
}

void PresetManager::setupNestedFM(Sound* sound, LiveController& controller) {
    // Create main FM synthesizer
    auto mainFM = std::make_unique<FMSynthesizer>(44100.0);
    
    // Create nested FM as carrier
    auto nestedFM = std::make_unique<FMSynthesizer>(44100.0);
    {
        auto nestedCarrier = std::make_unique<SineOscillator>(44100.0);
        nestedCarrier->setFrequency(440.0);
        nestedCarrier->setAmplitude(1.0);
        
        auto nestedModulator = std::make_unique<SineOscillator>(44100.0);
        nestedModulator->setFrequency(880.0);
        nestedModulator->setAmplitude(1.0);
        
        nestedFM->setCarrierOscillator(std::move(nestedCarrier));
        nestedFM->setModulatorOscillator(std::move(nestedModulator));
    }
    
    // Use nested FM as carrier for main FM
    mainFM->setCarrierOscillator(std::move(nestedFM));
    
    // Create simple modulator for main FM
    auto mainModulator = std::make_unique<SineOscillator>(44100.0);
    mainModulator->setFrequency(110.0);
    mainModulator->setAmplitude(1.0);
    mainFM->setModulatorOscillator(std::move(mainModulator));
    
    mainFM->registerParameters(controller);
    sound->addOscillator(std::move(mainFM));
}
