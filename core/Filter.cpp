#include "Filter.h"
#include "../interface/LiveController.h"

Filter::Filter(double sampleRate) : sampleRate(sampleRate) {
}

void Filter::processBuffer(double* buffer, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        buffer[i] = processSample(buffer[i]);
    }
}

void Filter::setSampleRate(double rate) {
    sampleRate = rate;
}

double Filter::getSampleRate() const {
    return sampleRate;
}

void Filter::addParameterWithPrefix(LiveController& controller, const std::string& prefix,
                                   const std::string& name, double* valuePtr, double minVal, 
                                   double maxVal, double step, std::function<void()> callback) {
    // Create full parameter name with prefix
    std::string fullName = prefix + " " + name;
    controller.addParameter(fullName, valuePtr, minVal, maxVal, step);
    
    // Connect the callback to the parameter
    int paramIndex = controller.getParameterCount() - 1;
    if (callback && paramIndex >= 0) {
        controller.setParameterCallback(paramIndex, callback);
    }
}

void Filter::addParameter(LiveController& controller, const std::string& name, 
                         double* valuePtr, double minVal, double maxVal, double step,
                         std::function<void()> callback) {
    // Create parameter name with filter type prefix
    std::string fullName = getTypeName() + " " + name;
    controller.addParameter(fullName, valuePtr, minVal, maxVal, step);
    
    // Connect the callback to the parameter
    int paramIndex = controller.getParameterCount() - 1;
    if (callback && paramIndex >= 0) {
        controller.setParameterCallback(paramIndex, callback);
    }
}
