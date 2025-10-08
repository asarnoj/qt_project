#include "Envelope.h"
#include <algorithm>

Envelope::Envelope(double sr)
    : sampleRate(sr), attack(10.0), decay(100.0), sustain(70.0), release(200.0),
      state(Idle), currentValue(0.0), samplesInStage(0), stageSampleCount(0),
      releaseStartValue(0.0) {}

void Envelope::setADSR(double a, double d, double s, double r) {
    attack = std::max(0.0, a);
    decay = std::max(0.0, d);
    sustain = std::clamp(s, 0.0, 100.0); // percent
    release = std::max(0.0, r);
}

void Envelope::noteOn() {
    enterStage(Attack);
}

void Envelope::noteOff() {
    releaseStartValue = currentValue;
    enterStage(Release);
}

void Envelope::enterStage(State newState) {
    state = newState;
    samplesInStage = 0;
    switch (state) {
        case Attack:   stageSampleCount = static_cast<int>((attack / 1000.0) * sampleRate); break;
        case Decay:    stageSampleCount = static_cast<int>((decay / 1000.0) * sampleRate); break;
        case Sustain:  stageSampleCount = 0; break;
        case Release:  stageSampleCount = static_cast<int>((release / 1000.0) * sampleRate); break;
        default:       stageSampleCount = 0; break;
    }
}

double Envelope::nextSample() {
    // Sustain is stored as percent, convert to 0.0-1.0 for calculations
    double sustainNorm = sustain / 100.0;
    switch (state) {
        case Idle:
            currentValue = 0.0;
            break;
        case Attack:
            if (stageSampleCount > 0)
                currentValue = static_cast<double>(samplesInStage) / stageSampleCount;
            else
                currentValue = 1.0;
            if (++samplesInStage >= stageSampleCount)
                enterStage(Decay);
            break;
        case Decay:
            if (stageSampleCount > 0)
                currentValue = 1.0 + (sustainNorm - 1.0) * (static_cast<double>(samplesInStage) / stageSampleCount);
            else
                currentValue = sustainNorm;
            if (++samplesInStage >= stageSampleCount)
                enterStage(Sustain);
            break;
        case Sustain:
            currentValue = sustainNorm;
            break;
        case Release:
            if (stageSampleCount > 0)
                currentValue = releaseStartValue * (1.0 - static_cast<double>(samplesInStage) / stageSampleCount);
            else
                currentValue = 0.0;
            if (++samplesInStage >= stageSampleCount)
                enterStage(Idle);
            break;
    }
    return currentValue;
}

double* Envelope::getAttackPtr() { return &attack; }
double* Envelope::getDecayPtr() { return &decay; }
double* Envelope::getSustainPtr() { return &sustain; }
double* Envelope::getReleasePtr() { return &release; }

bool Envelope::isActive() const {
    return state != Idle;
}
