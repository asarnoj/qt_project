#ifndef SYNTHESIZERWINDOW_H
#define SYNTHESIZERWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QScrollArea>
#include <QSlider>
#include <QLineEdit>
#include <QDoubleValidator> 
#include <vector>
#include "../interface/LiveController.h"
#include "../presets/PresetManager.h"
#include "../audio/AudioEngine.h"

class SynthesizerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit SynthesizerWindow(QWidget* parent = nullptr);
    ~SynthesizerWindow();

private slots:
    void onLoadPreset();
    void onPlay();
    void onStop();
    void syncAudioParameters();

private:
    // GUI Components
    QWidget* mainWidget;
    QVBoxLayout* mainLayout;
    QComboBox* presetSelector;
    QPushButton* loadButton;
    QPushButton* playButton;
    QPushButton* stopButton;
    QLabel* controlsLabel;
    
    // Scrolling components
    QScrollArea* scrollArea;
    QWidget* scrollContent;
    QVBoxLayout* scrollLayout;
    
    // Parameter controls (dynamic)
    std::vector<QHBoxLayout*> parameterLayouts;
    std::vector<QPushButton*> controlButtons;
    
    // Core systems
    AudioEngine audioEngine;
    LiveController controller;
    PresetManager presetManager;
    
    // Timer for audio sync
    QTimer* audioSyncTimer;
    
    // Playback state
    bool isPlaying;
    
    // Methods
    void setupUI();
    void setupAudio();
    void clearDynamicControls();
    void createParameterControls();
    void createParameterGroup(const std::string& groupName, const std::vector<int>& paramIndices);
    void createSingleParameter(int paramIndex, int indentLevel);
};

#endif // SYNTHESIZERWINDOW_H
