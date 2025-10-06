#ifndef ADDITIVESYNTHESIZER_H
#define ADDITIVESYNTHESIZER_H

#include "../core/Oscillator.h"
#include <vector>
#include <memory>

class AdditiveSynthesizer : public Oscillator {
public:
    AdditiveSynthesizer(double sampleRate = 44100.0);

    // Add an oscillator to the additive synth
    void addOscillator(std::unique_ptr<Oscillator> osc);

    // Remove all oscillators
    void clearOscillators();

    // Get number of oscillators
    size_t getOscillatorCount() const;

    // Main sample generation
    double nextSample() override;

    // Parameter registration
    void registerParameters(LiveController& controller) override;
    void registerParametersWithPrefix(LiveController& controller, const std::string& prefix) override;
    std::string getTypeName() const override { return "Additive"; }

private:
    std::vector<std::unique_ptr<Oscillator>> oscillators;
    double amplitude; // Output amplitude normalization
};

#endif // ADDITIVESYNTHESIZER_H
