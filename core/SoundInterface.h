#ifndef SOUNDINTERFACE_H
#define SOUNDINTERFACE_H

#include <vector>
#include <memory>
#include <map>
#include "SoundPreset.h"
#include "Sound.h"

class SoundInterface {
public:
    SoundInterface(Sound* sound);
    
    // Preset management
    void registerPreset(const std::string& name, std::function<void(Sound*, SoundPreset*)> setupFunction);
    void loadPreset(const std::string& name);
    void loadPreset(int index);
    
    // Parameter control
    void setParameter(int index, double value);
    void setParameter(const std::string& name, double value);
    
    // Interface info
    const std::vector<std::string> getPresetNames() const;
    const SoundPreset* getCurrentPreset() const { return currentPreset.get(); }
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    
    // Console interface
    void printCurrentParameters() const;
    void printAvailablePresets() const;

private:
    Sound* sound;
    std::vector<std::unique_ptr<SoundPreset>> presets;
    std::unique_ptr<SoundPreset> currentPreset;
    int currentPresetIndex;
};

#endif // SOUNDINTERFACE_H
