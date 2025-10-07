#include "Envelope.h"
#include <algorithm>

Envelope::Envelope(double sr)
    : sampleRate(sr), attack(0.01), decay(0.1), sustain(0.7), release(0.2),
      state(Idle), currentValue(0.0), samplesInStage(0), stageSampleCount(0) {}

void Envelope::setADSR(double a, double d, double s, double r) {
    attack = std::max(0.001, a);
    decay = std::max(0.001, d);
    sustain = std::clamp(s, 0.0, 1.0);
    release = std::max(0.001, r);
}

void Envelope::noteOn() {
    enterStage(Attack);
}

void Envelope::noteOff() {
    enterStage(Release);
}

void Envelope::enterStage(State newState) {
    state = newState;
    samplesInStage = 0;
    switch (state) {
        case Attack:   stageSampleCount = static_cast<int>(attack * sampleRate); break;
        case Decay:    stageSampleCount = static_cast<int>(decay * sampleRate); break;
        case Sustain:  stageSampleCount = 0; break;
        case Release:  stageSampleCount = static_cast<int>(release * sampleRate); break;
        default:       stageSampleCount = 0; break;
    }
}

double Envelope::nextSample() {
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
                currentValue = 1.0 + (sustain - 1.0) * (static_cast<double>(samplesInStage) / stageSampleCount);
            else
                currentValue = sustain;
            if (++samplesInStage >= stageSampleCount)
                enterStage(Sustain);
            break;
        case Sustain:
            currentValue = sustain;
            break;
        case Release:
            if (stageSampleCount > 0)
                currentValue = currentValue * (1.0 - static_cast<double>(samplesInStage) / stageSampleCount);
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
