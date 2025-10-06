#include "SoundInterface.h"
#include <iostream>
#include <iomanip>

SoundInterface::SoundInterface(Sound* sound) 
    : sound(sound), currentPresetIndex(-1) {
}

void SoundInterface::registerPreset(const std::string& name, 
                                   std::function<void(Sound*, SoundPreset*)> setupFunction) {
    
    auto preset = std::make_unique<SoundPreset>(name, [setupFunction](Sound* s) {
        // This will be called later when we have access to the preset object
    });
    
    // Store the setup function for later use
    presets.push_back(std::move(preset));
}

void SoundInterface::loadPreset(const std::string& name) {
    for (int i = 0; i < static_cast<int>(presets.size()); ++i) {
        if (presets[i]->getName() == name) {
            loadPreset(i);
            return;
        }
    }
    std::cout << "Preset '" << name << "' not found!" << std::endl;
}

void SoundInterface::loadPreset(int index) {
    if (index >= 0 && index < static_cast<int>(presets.size())) {
        currentPresetIndex = index;
        currentPreset = std::make_unique<SoundPreset>(*presets[index]);
        currentPreset->setupSound(sound);
        
        std::cout << "✅ Loaded preset: " << currentPreset->getName() << std::endl;
        printCurrentParameters();
    }
}

void SoundInterface::setParameter(int index, double value) {
    if (currentPreset) {
        currentPreset->setParameter(index, value);
        std::cout << "📊 Parameter " << index << " set to " << value << std::endl;
    }
}

void SoundInterface::setParameter(const std::string& name, double value) {
    if (currentPreset) {
        currentPreset->setParameter(name, value);
        std::cout << "📊 " << name << " set to " << value << std::endl;
    }
}

const std::vector<std::string> SoundInterface::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : presets) {
        names.push_back(preset->getName());
    }
    return names;
}

void SoundInterface::printCurrentParameters() const {
    if (!currentPreset) {
        std::cout << "No preset loaded." << std::endl;
        return;
    }
    
    const auto& params = currentPreset->getParameters();
    std::cout << "\n🎛️  Current Parameters:" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (int i = 0; i < static_cast<int>(params.size()); ++i) {
        const auto& p = params[i];
        std::cout << std::setw(3) << i << ". " 
                  << std::setw(20) << p.name << ": "
                  << std::setw(8) << std::fixed << std::setprecision(2) << p.currentValue 
                  << " " << p.unit
                  << " [" << p.minValue << "-" << p.maxValue << "]" << std::endl;
    }
    std::cout << std::string(50, '-') << std::endl;
}

void SoundInterface::printAvailablePresets() const {
    std::cout << "\n🎵 Available Presets:" << std::endl;
    for (int i = 0; i < static_cast<int>(presets.size()); ++i) {
        std::cout << "  " << i << ". " << presets[i]->getName() << std::endl;
    }
}
