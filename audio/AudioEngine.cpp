#include "AudioEngine.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QTimer>
#include <QDebug>

class AudioIODevice : public QIODevice {
public:
    explicit AudioIODevice(Sound* soundSystem, QObject* parent = nullptr) 
        : QIODevice(parent), sound(soundSystem), sampleCount(0) {
    }

    qint64 readData(char* data, qint64 maxlen) override {
        qint64 samples = maxlen / sizeof(float);
        float* buffer = reinterpret_cast<float*>(data);

        for (qint64 i = 0; i < samples; ++i) {
            double sample = sound->nextSample();
            buffer[i] = static_cast<float>(sample);
            sampleCount++;
        }

        return maxlen;
    }

    qint64 writeData(const char*, qint64) override { 
        return 0;
    }

    bool isSequential() const override {
        return true;
    }

    qint64 bytesAvailable() const override {
        return QIODevice::bytesAvailable() + 1000000;
    }

    bool atEnd() const override {
        return false;
    }

private:
    Sound* sound;
    int sampleCount;
};

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent), audioOutput(nullptr), ioDevice(nullptr) {
    // Create Sound system
    sound = std::make_unique<Sound>(44100.0);
}

void AudioEngine::start() {
    // Set up audio format
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Float);
    
    // Get default audio device
    QAudioDevice audioDevice = QMediaDevices::defaultAudioOutput();
    
    // Check if we have a valid audio device
    if (!audioDevice.isNull()) {
        // Check format support
        if (!audioDevice.isFormatSupported(format)) {
            qWarning() << "Warning: Default format not supported - trying to use nearest";
            format = audioDevice.preferredFormat();
        }
        
        // Create audio sink
        audioOutput = new QAudioSink(audioDevice, format, this);
        audioOutput->setVolume(1.0);
        
        // Create audio device
        ioDevice = new AudioIODevice(sound.get(), this);
        ioDevice->open(QIODevice::ReadOnly);
        
        // Start audio
        audioOutput->start(ioDevice);
        
        // Monitor state
        connect(audioOutput, &QAudioSink::stateChanged, this, [this](QAudio::State state) {
            switch (state) {
                case QAudio::IdleState:
                    qDebug() << "Audio system idle";
                    break;
                case QAudio::StoppedState:
                    // Check for errors
                    if (audioOutput->error() != QAudio::NoError) {
                        qWarning() << "Audio output error:" << audioOutput->error();
                    }
                    break;
                default:
                    break;
            }
        });
    } else {
        qWarning() << "No audio output device found!";
    }
}

void AudioEngine::stop() {
    if (audioOutput) {
        audioOutput->stop();
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
    
    if (ioDevice) {
        ioDevice->close();
        ioDevice->deleteLater();
        ioDevice = nullptr;
    }
}