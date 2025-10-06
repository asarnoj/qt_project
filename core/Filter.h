#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <functional>

// Forward declaration
class LiveController;

class Filter {
public:
    Filter(double sampleRate = 44100.0);
    virtual ~Filter() = default;

    // Core filter functionality - process input signal instead of generating
    virtual double processSample(double input) = 0;
    
    // Buffer processing
    virtual void processBuffer(double* buffer, int numSamples);
    
    // Sample rate management
    void setSampleRate(double rate);
    double getSampleRate() const;
    
    // Filter state management
    virtual void reset() = 0;
    
    // Automatic parameter registration
    virtual void registerParameters(LiveController& controller) = 0;
    
    // Register parameters with a prefix for nested filters
    virtual void registerParametersWithPrefix(LiveController& controller, const std::string& /* prefix */) {
        // Default implementation - only register if not used as component
        if (!isUsedAsComponent) {
            registerParameters(controller);
        }
    }
    
    // Mark this filter as being used as a component
    void setUsedAsComponent(bool isComponent = true) { isUsedAsComponent = isComponent; }
    bool getIsUsedAsComponent() const { return isUsedAsComponent; }

    // Get filter type name for display
    virtual std::string getTypeName() const = 0;

protected:
    double sampleRate;
    
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

#endif // FILTER_H
