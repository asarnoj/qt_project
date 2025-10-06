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
    // amplitude is automatically 1.0
    sine->registerParameters(controller);
    sound->addOscillator(std::move(sine));
    
    // Add master volume control with proper callback to Sound object - FIXED
    // Use Sound's own master volume pointer directly instead of creating a local variable
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0.0, 1.0, 0.05);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                   [sound]() {
                                       // No need to dereference a pointer, just use the value directly
                                       // since setMasterVolume will be working with the same memory location
                                       sound->updateMasterVolume();
                                   });
}

void PresetManager::setupBasicFM(Sound* sound, LiveController& controller) {
    auto fmSynth = std::make_unique<FMSynthesizer>(44100.0);
    
    auto carrier = std::make_unique<SineOscillator>(44100.0);
    carrier->setFrequency(440.0);
    // amplitude automatically 1.0
    
    auto modulator = std::make_unique<SineOscillator>(44100.0);
    modulator->setFrequency(880.0);
    // amplitude automatically 1.0
    
    fmSynth->setCarrierOscillator(std::move(carrier));
    fmSynth->setModulatorOscillator(std::move(modulator));
    fmSynth->registerParameters(controller);
    
    sound->addOscillator(std::move(fmSynth));
    
    // Add master volume control with proper callback - FIXED
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0.0, 1.0, 0.05);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                   [sound]() {
                                       sound->updateMasterVolume();
                                   });
}

void PresetManager::setupNestedFM(Sound* sound, LiveController& controller) {
    // Create main FM synthesizer
    auto mainFM = std::make_unique<FMSynthesizer>(44100.0);
    
    // Create simple sine oscillator as carrier
    auto carrier = std::make_unique<SineOscillator>(44100.0);
    carrier->setFrequency(440.0);
    // amplitude automatically 1.0
    mainFM->setCarrierOscillator(std::move(carrier));
    
    // Create nested FM as modulator
    auto nestedFM = std::make_unique<FMSynthesizer>(44100.0);
    {
        auto nestedCarrier = std::make_unique<SineOscillator>(44100.0);
        nestedCarrier->setFrequency(220.0); // Changed frequency for better sound
        // amplitude automatically 1.0
        
        auto nestedModulator = std::make_unique<SineOscillator>(44100.0);
        nestedModulator->setFrequency(55.0); // Changed frequency for better sound
        // amplitude automatically 1.0
        
        nestedFM->setCarrierOscillator(std::move(nestedCarrier));
        nestedFM->setModulatorOscillator(std::move(nestedModulator));
        nestedFM->setModulationDepth(50.0); // Set reasonable default depth
    }
    
    // Set the nested FM as the modulator for main FM
    mainFM->setModulatorOscillator(std::move(nestedFM));
    mainFM->setModulationDepth(30.0); // Lower depth for more subtle effect
    
    // Register parameters in a way that clearly shows the hierarchical structure
    mainFM->registerParameters(controller);
    
    sound->addOscillator(std::move(mainFM));
    
    // Add master volume control with proper callback
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0.0, 1.0, 0.05);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                  [sound]() {
                                      sound->updateMasterVolume();
                                  });
}