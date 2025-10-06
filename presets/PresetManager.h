#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "../core/Sound.h"
#include "../interface/LiveController.h"

class PresetManager {
public:
    using PresetSetupFunction = std::function<void(Sound*, LiveController&)>;
    
    struct Preset {
        std::string name;
        std::string description;
        PresetSetupFunction setupFunction;
    };
    
    PresetManager();
    
    // Preset management
    void registerPreset(const std::string& name, const std::string& description, PresetSetupFunction setupFunc);
    void loadPreset(int index, Sound* sound, LiveController& controller);
    
    // Getters
    const std::vector<Preset>& getPresets() const { return presets; }
    std::vector<std::string> getPresetNames() const;
    int getPresetCount() const { return static_cast<int>(presets.size()); }

private:
    std::vector<Preset> presets;
    
    // Built-in preset setup functions
    static void setupSimpleSine(Sound* sound, LiveController& controller);
    static void setupSimpleSaw(Sound* sound, LiveController& controller);
    static void setupBasicFM(Sound* sound, LiveController& controller);
    static void setupNestedFM(Sound* sound, LiveController& controller);
};

#endif // PRESETMANAGER_H
