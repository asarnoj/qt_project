#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H

#include "../core/Filter.h"

class LowPassFilter : public Filter {
public:
    LowPassFilter(double sampleRate = 44100.0);

    double processSample(double input) override;

    void setCutoffFrequency(double freq);
    double getCutoffFrequency() const { return cutoffFrequency; }

    void setResonance(double q);
    double getResonance() const { return resonance; }

    void registerParameters(LiveController& controller) override;
    void registerParametersWithPrefix(LiveController& controller, const std::string& prefix) override;
    std::string getTypeName() const override { return "LowPass"; }

    void reset() override;

private:
    double cutoffFrequency;
    double resonance; // Q

    // Biquad coefficients
    double b0, b1, b2;
    double a1, a2;

    // Delay line
    double x1, x2;
    double y1, y2;

    void updateCoefficients();
};

#endif // LOWPASSFILTER_H
