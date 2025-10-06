#include "SoundPreset.h"
#include <algorithm>

SoundPreset::SoundPreset(const std::string& name, std::function<void(Sound*)> setupFunction)
    : name(name), setupFunction(setupFunction), currentSound(nullptr) {
}

void SoundPreset::setupSound(Sound* sound) {
    currentSound = sound;
    parameters.clear();
    
    // Call the setup function (it will register parameters via callbacks)
    setupFunction(sound);
}

void SoundPreset::addParameter(const std::string& name, ParameterType type, 
                              double current, double min, double max, 
                              const std::string& unit, std::function<void(double)> setter) {
    Parameter param;
    param.name = name;
    param.type = type;
    param.currentValue = current;
    param.minValue = min;
    param.maxValue = max;
    param.unit = unit;
    param.setter = setter;
    
    parameters.push_back(param);
}

void SoundPreset::setParameter(int index, double value) {
    if (index >= 0 && index < static_cast<int>(parameters.size())) {
        // Clamp value to valid range
        value = std::max(parameters[index].minValue, 
                        std::min(parameters[index].maxValue, value));
        
        parameters[index].currentValue = value;
        parameters[index].setter(value);
    }
}

void SoundPreset::setParameter(const std::string& name, double value) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
        [&name](const Parameter& p) { return p.name == name; });
    
    if (it != parameters.end()) {
        int index = std::distance(parameters.begin(), it);
        setParameter(index, value);
    }
}
