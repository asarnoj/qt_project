#ifndef BANDPASSFILTER_H
#define BANDPASSFILTER_H

#include "../core/Filter.h"

class BandPassFilter : public Filter {
public:
    BandPassFilter(double sampleRate = 44100.0);
    
    // Core filter functionality - process input signal
    double processSample(double input) override;
    
    // Filter parameters
    void setTargetFrequency(double freq);
    void setBandwidth(double bandwidth);
    double getTargetFrequency() const { return targetFrequency; }
    double getBandwidth() const { return bandwidth; }
    
    // Parameter registration
    void registerParameters(LiveController& controller) override;
    void registerParametersWithPrefix(LiveController& controller, const std::string& prefix) override;
    std::string getTypeName() const override { return "BandPass"; }
    
    // Reset filter state
    void reset() override;

private:
    // Filter parameters
    double targetFrequency;
    double bandwidth;
    
    // Biquad filter coefficients
    double b0, b1, b2;  // feedforward coefficients
    double a1, a2;      // feedback coefficients
    
    // Filter state (delay line)
    double x1, x2;      // input history
    double y1, y2;      // output history
    
    // Update filter coefficients when parameters change
    void updateCoefficients();
};

#endif // BANDPASSFILTER_H
