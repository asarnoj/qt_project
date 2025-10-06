#ifndef SYNTHESIZERWINDOW_H
#define SYNTHESIZERWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
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
    void updateDisplay();
    void syncAudioParameters();

private:
    // GUI Components
    QWidget* mainWidget;
    QVBoxLayout* mainLayout;
    QTextEdit* parameterDisplay;
    QComboBox* presetSelector;
    QPushButton* loadButton;
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
    
    // Timers
    QTimer* displayUpdateTimer;
    QTimer* audioSyncTimer;
    
    // Methods
    void setupUI();
    void setupAudio();
    void clearDynamicControls();
    void createParameterControls();
    void createParameterGroup(const std::string& groupName, const std::vector<int>& paramIndices);
    void createSingleParameter(int paramIndex, int indentLevel);
};

#endif // SYNTHESIZERWINDOW_H
