#ifndef LIVECONTROLLER_H
#define LIVECONTROLLER_H

#include <vector>
#include <string>
#include <functional>

// Simple parameter for live control
struct LiveParameter {
    std::string name;
    double* valuePtr;           // Direct pointer to the value to control
    double minValue;
    double maxValue;
    double step;               // How much to change per adjustment
    
    LiveParameter(const std::string& n, double* ptr, double min, double max, double s = 0.1)
        : name(n), valuePtr(ptr), minValue(min), maxValue(max), step(s) {}
};

class LiveController {
public:
    LiveController();
    
    // Parameter management
    void addParameter(const std::string& name, double* valuePtr, 
                     double minValue, double maxValue, double step = 0.1);
    void clearParameters();
    
    // Control functions
    void increaseParameter(int index);
    void decreaseParameter(int index);
    void setParameter(int index, double value);
    
    // Info functions
    void printParameters() const;
    void printControls() const;
    int getParameterCount() const { return parameters.size(); }
    const LiveParameter& getParameter(int index) const { return parameters[index]; }

private:
    std::vector<LiveParameter> parameters;
    
    void clampValue(int index);
};

#endif // LIVECONTROLLER_H
