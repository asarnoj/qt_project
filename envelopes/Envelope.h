#pragma once

class Envelope {
public:
    Envelope(double sampleRate = 44100.0);

    void setADSR(double attack, double decay, double sustain, double release);

    void noteOn();
    void noteOff();

    double nextSample();

    // For parameter registration
    double* getAttackPtr();
    double* getDecayPtr();
    double* getSustainPtr();
    double* getReleasePtr();

    bool isActive() const;

private:
    double sampleRate;
    double attack;
    double decay;
    double sustain;
    double release;
    enum State { Idle, Attack, Decay, Sustain, Release };
    State state;
    double currentValue;
    int samplesInStage;
    int stageSampleCount;

    void enterStage(State newState);
};
