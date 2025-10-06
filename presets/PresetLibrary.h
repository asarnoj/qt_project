#ifndef PRESETLIBRARY_H
#define PRESETLIBRARY_H

#include "../core/SoundInterface.h"

class PresetLibrary {
public:
    static void registerAllPresets(SoundInterface& interface);
    
private:
    // Individual preset setup functions
    static void setupSimpleSine(Sound* sound, SoundInterface& interface);
    static void setupHarmony(Sound* sound, SoundInterface& interface);  
    static void setupFMExploration(Sound* sound, SoundInterface& interface);
    static void setupLayeredTexture(Sound* sound, SoundInterface& interface);
};

#endif // PRESETLIBRARY_H
