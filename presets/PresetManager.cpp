#include "PresetManager.h"
#include "../oscillators/SineOscillator.h"
#include "../oscillators/SawOscillator.h"
#include "../synthesizers/FMSynthesizer.h"
#include "../filters/BandPassFilter.h"
#include "../synthesizers/AdditiveSynthesizer.h" // Add this include
#include "../envelopes/Envelope.h"
#include <iostream>

PresetManager::PresetManager() {
    // Register all built-in presets
    registerPreset("Simple Sine Wave", "Single sine oscillator with frequency and amplitude control", setupSimpleSine);
    registerPreset("Simple Saw Wave", "Single sawtooth oscillator with frequency and amplitude control", setupSimpleSaw);
    registerPreset("FM Synthesizer", "Basic FM synthesis with sine carrier and modulator", setupBasicFM);
    registerPreset("Nested FM", "Advanced nested FM synthesis for complex timbres", setupNestedFM);
    registerPreset("Triple Bandpass Additive", "Saw wave split into three bandpass filters, summed additively", setupTripleBandpassAdditive);
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
    sine->registerParameters(controller);

    // Create and register envelope (times in ms, sustain in percent)
    auto envelope = std::make_unique<Envelope>(44100.0);
    envelope->setADSR(10.0, 100.0, 70.0, 200.0); // ms, ms, %, ms

    // Register envelope parameters (sustain in percent)
    controller.addParameter("Attack", envelope->getAttackPtr(), 0.0, 2000.0, 10.0);
    controller.addParameter("Decay", envelope->getDecayPtr(), 0.0, 2000.0, 100.0);
    controller.addParameter("Sustain", envelope->getSustainPtr(), 0.0, 100.0, 70.0);
    controller.addParameter("Release", envelope->getReleasePtr(), 0.0, 2000.0, 200.0);

    sound->addOscillator(std::move(sine));
    sound->addEnvelope(std::move(envelope));

    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0, 100, 50);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                   [sound]() {
                                       sound->updateMasterVolume();
                                   });
}

void PresetManager::setupSimpleSaw(Sound* sound, LiveController& controller) {
    auto saw = std::make_unique<SawOscillator>(44100.0);
    saw->setFrequency(440.0);
    // amplitude is automatically 1.0
    saw->registerParameters(controller);
    
    // Create a BandPassFilter for the saw wave
    auto bandpass = std::make_unique<BandPassFilter>(44100.0);
    bandpass->setTargetFrequency(1000.0);  // Start with 1kHz center frequency
    bandpass->setBandwidth(300.0);         // 300Hz bandwidth
    bandpass->registerParameters(controller);
    
    // Add both the oscillator and filter to the sound
    sound->addOscillator(std::move(saw));
    sound->addFilter(std::move(bandpass));
    
    // Add master volume control with consistent 0-100 range
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0, 100, 50);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                   [sound]() {
                                       sound->updateMasterVolume();
                                   });
}

void PresetManager::setupBasicFM(Sound* sound, LiveController& controller) {
    auto fmSynth = std::make_unique<FMSynthesizer>(44100.0);
    
    auto carrier = std::make_unique<SawOscillator>(44100.0);
    carrier->setFrequency(440.0);
    // amplitude automatically 1.0
    
    auto modulator = std::make_unique<SawOscillator>(44100.0);
    modulator->setFrequency(880.0);
    // amplitude automatically 1.0
    
    fmSynth->setCarrierOscillator(std::move(carrier));
    fmSynth->setModulatorOscillator(std::move(modulator));
    fmSynth->registerParameters(controller);
    
    sound->addOscillator(std::move(fmSynth));
    
    // Add master volume control with consistent 0-100 range
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0, 100, 50);
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
    // amplitude is automatically 1.0
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
    
    // Add master volume control with consistent 0-100 range (was already correct)
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0, 100, 50);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                  [sound]() {
                                      sound->updateMasterVolume();
                                  });
}

void PresetManager::setupTripleBandpassAdditive(Sound* sound, LiveController& controller) {
    // Frequencies and bandwidths for each band
    std::vector<double> centerFreqs = {600.0, 1200.0, 2400.0};
    std::vector<double> bandwidths = {200.0, 300.0, 400.0};

    // Prepare additive synth
    auto additive = std::make_unique<AdditiveSynthesizer>(44100.0);

    // For each band, create an independent saw oscillator and bandpass filter
    for (int i = 0; i < 3; ++i) {
        // Create independent saw oscillator
        auto saw = std::make_unique<SawOscillator>(44100.0);
        saw->setFrequency(440.0);
        saw->registerParametersWithPrefix(controller, "Saw " + std::to_string(i + 1));

        // Create bandpass filter
        auto filter = std::make_unique<BandPassFilter>(44100.0);
        filter->setTargetFrequency(centerFreqs[i]);
        filter->setBandwidth(bandwidths[i]);
        filter->registerParametersWithPrefix(controller, "Bandpass " + std::to_string(i + 1));

        // Create a wrapper oscillator that applies the filter to the saw
        struct FilteredOsc : public Oscillator {
            std::unique_ptr<SawOscillator> saw;
            std::unique_ptr<BandPassFilter> filter;
            FilteredOsc(std::unique_ptr<SawOscillator> s, std::unique_ptr<BandPassFilter> f, double sr)
                : Oscillator(sr), saw(std::move(s)), filter(std::move(f)) {}
            double nextSample() override {
                return filter->processSample(saw->nextSample());
            }
            void registerParameters(LiveController&) override {}
            void registerParametersWithPrefix(LiveController&, const std::string&) override {}
            std::string getTypeName() const override { return "FilteredOsc"; }
        };

        // Add the filtered oscillator to the additive synth
        auto filteredOsc = std::make_unique<FilteredOsc>(std::move(saw), std::move(filter), 44100.0);
        additive->addOscillator(std::move(filteredOsc));
    }

    // Register additive synth parameters
    additive->registerParameters(controller);
    sound->addOscillator(std::move(additive));

    // Add master volume control
    double* masterVolumePtr = sound->getMasterVolumePtr();
    controller.addParameter("Master Volume", masterVolumePtr, 0, 100, 50);
    controller.setParameterCallback(controller.getParameterCount() - 1, 
                                   [sound]() {
                                       sound->updateMasterVolume();
                                   });
}