#include "BandPassFilter.h"
#include "../interface/LiveController.h"
#include <iostream>
#include <cmath>
#include <algorithm>

BandPassFilter::BandPassFilter(double sampleRate) 
    : Filter(sampleRate), targetFrequency(1000.0), bandwidth(200.0),
      b0(0.0), b1(0.0), b2(0.0), a1(0.0), a2(0.0),
      x1(0.0), x2(0.0), y1(0.0), y2(0.0) {
    
    // Initialize filter coefficients
    updateCoefficients();
}

double BandPassFilter::processSample(double input) {
    // Biquad difference equation: y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
    double output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    
    // Update delay line
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    
    return output;
}

void BandPassFilter::setTargetFrequency(double freq) {
    targetFrequency = std::max(1.0, std::min(freq, sampleRate * 0.45)); // Nyquist limit with safety margin
    updateCoefficients();
}

void BandPassFilter::setBandwidth(double bw) {
    bandwidth = std::max(1.0, std::min(bw, sampleRate * 0.4)); // Reasonable bandwidth limits
    updateCoefficients();
}

void BandPassFilter::updateCoefficients() {
    // Calculate normalized frequency and Q factor
    double omega = 2.0 * M_PI * targetFrequency / sampleRate;
    double Q = targetFrequency / bandwidth;
    
    // Limit Q to prevent instability
    Q = std::max(0.1, std::min(Q, 30.0));
    
    // Calculate filter coefficients for band-pass filter
    double cosOmega = cos(omega);
    double sinOmega = sin(omega);
    double alpha = sinOmega / (2.0 * Q);
    
    // Normalize coefficients
    double norm = 1.0 + alpha;
    
    // Feedforward coefficients
    b0 = alpha / norm;
    b1 = 0.0 / norm;
    b2 = -alpha / norm;
    
    // Feedback coefficients
    a1 = -2.0 * cosOmega / norm;
    a2 = (1.0 - alpha) / norm;
}

void BandPassFilter::reset() {
    x1 = x2 = y1 = y2 = 0.0;
}

void BandPassFilter::registerParameters(LiveController& controller) {
    registerParametersWithPrefix(controller, getTypeName());
}

void BandPassFilter::registerParametersWithPrefix(LiveController& controller, const std::string& prefix) {
    std::cout << "🎛️ " << prefix << " registering filter parameters..." << std::endl;
    
    // Register target frequency parameter
    addParameterWithPrefix(controller, prefix, "Target Freq", &targetFrequency, 
                          20.0, 8000.0, 50.0,
                          [this]() {
                              setTargetFrequency(targetFrequency);
                          });

    // Register bandwidth parameter
    addParameterWithPrefix(controller, prefix, "Bandwidth", &bandwidth, 
                          10.0, 2000.0, 200.0,
                          [this]() {
                              setBandwidth(bandwidth);
                          });
}
