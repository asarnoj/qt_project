#ifndef SOUNDPRESET_H
#define SOUNDPRESET_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "Sound.h"

// Parameter types that can be controlled
enum class ParameterType {
    FREQUENCY,
    AMPLITUDE,
    FM_MODULATION_DEPTH,
    FM_MODULATOR_FREQ,
    MASTER_VOLUME
};

// A controllable parameter
struct Parameter {
    std::string name;           // "Bass Frequency", "FM Depth", etc.
    ParameterType type;
    double currentValue;
    double minValue;
    double maxValue;
    std::string unit;           // "Hz", "", "dB", etc.
    
    // Function to apply this parameter change
    std::function<void(double)> setter;
};

// A sound preset that knows its parameters
class SoundPreset {
public:
    SoundPreset(const std::string& name, std::function<void(Sound*)> setupFunction);
    
    // Setup the sound and discover all parameters
    void setupSound(Sound* sound);
    
    // Parameter management
    const std::vector<Parameter>& getParameters() const { return parameters; }
    void setParameter(int index, double value);
    void setParameter(const std::string& name, double value);
    
    // Preset info
    const std::string& getName() const { return name; }
    
    // Add a parameter (called during setup)
    void addParameter(const std::string& name, ParameterType type, 
                     double current, double min, double max, 
                     const std::string& unit, std::function<void(double)> setter);

private:
    std::string name;
    std::function<void(Sound*)> setupFunction;
    std::vector<Parameter> parameters;
    Sound* currentSound;
};

#endif // SOUNDPRESET_H
