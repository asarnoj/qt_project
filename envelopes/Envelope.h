#pragma once

class Envelope {
public:
    Envelope(double sampleRate = 44100.0);

    // Attack, Decay, Release in ms; Sustain in percent (0-100)
    void setADSR(double attack, double decay, double sustain, double release);

    void noteOn();
    void noteOff();

    double nextSample();

    // For parameter registration (attack/decay/release in ms, sustain in percent)
    double* getAttackPtr();
    double* getDecayPtr();
    double* getSustainPtr();
    double* getReleasePtr();

    bool isActive() const;

private:
    double sampleRate;
    double attack;   // ms
    double decay;    // ms
    double sustain;  // percent (0-100)
    double release;  // ms
    enum State { Idle, Attack, Decay, Sustain, Release };
    State state;
    double currentValue;
    int samplesInStage;
    int stageSampleCount;
    double releaseStartValue;
    void enterStage(State newState);
};
