#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cmath>
#include <string>
#include <vector>
#include <functional>

// Forward declaration
class LiveController;

// Parameter info for automatic registration
struct OscillatorParameter {
    std::string name;
    double* valuePtr;
    double minValue;
    double maxValue;
    double step;
    std::function<void()> updateCallback; // Called when parameter changes
};

class Oscillator {
public:
    Oscillator(double sampleRate = 44100.0);
    virtual ~Oscillator() = default;

    virtual void setFrequency(double freq);  // Made virtual
    void setAmplitude(double amp);
    void setSampleRate(double rate);

    double getFrequency() const;
    double getAmplitude() const;
    double getSampleRate() const;

    // Generate one sample — implemented differently by each oscillator type
    virtual double nextSample() = 0;
    
    // Automatic parameter registration
    virtual void registerParameters(LiveController& controller) = 0;
    
    // New: Register parameters with a prefix for nested oscillators
    virtual void registerParametersWithPrefix(LiveController& controller, const std::string& /* prefix */) {
        // Default implementation - only register if not used as component
        if (!isUsedAsComponent) {
            registerParameters(controller);
        }
    }
    
    // Mark this oscillator as being used as a component (carrier/modulator)
    void setUsedAsComponent(bool isComponent = true) { isUsedAsComponent = isComponent; }
    bool getIsUsedAsComponent() const { return isUsedAsComponent; }

    // Get oscillator type name for display
    virtual std::string getTypeName() const = 0;

protected:
    double frequency;
    double amplitude;
    double sampleRate;
    double phase;  // 0 to 1 range
    
    // Helper to register a parameter with the controller
    void addParameter(LiveController& controller, const std::string& name, 
                     double* valuePtr, double minVal, double maxVal, double step,
                     std::function<void()> callback = nullptr);
    
    // Helper to register a parameter with custom prefix
    void addParameterWithPrefix(LiveController& controller, const std::string& prefix,
                               const std::string& name, double* valuePtr, double minVal, 
                               double maxVal, double step, std::function<void()> callback = nullptr);

private:
    bool isUsedAsComponent = false;  // Flag to prevent duplicate parameter registration
};

#endif // OSCILLATOR_H