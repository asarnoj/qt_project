#pragma once

#include <QObject>
#include <QAudioSink>
#include <QIODevice>
#include "../core/Sound.h"
#include <memory>

class AudioEngine : public QObject {
    Q_OBJECT
    
public:
    explicit AudioEngine(QObject* parent = nullptr);
    ~AudioEngine() = default;
    
    void start();
    void stop();
    
    Sound* getSound() { return sound.get(); }
    bool isRunning() const { return audioOutput != nullptr; }

private:
    QAudioSink* audioOutput;
    QIODevice* ioDevice;
    std::unique_ptr<Sound> sound;
};