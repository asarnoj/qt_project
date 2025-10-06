#include "SynthesizerWindow.h"
#include <QApplication>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

SynthesizerWindow::SynthesizerWindow(QWidget* parent)
    : QMainWindow(parent), mainWidget(nullptr), mainLayout(nullptr), isPlaying(false) {
    
    setupUI();
    setupAudio();
    
    // Load default preset
    presetManager.loadPreset(0, audioEngine.getSound(), controller);
    createParameterControls();
    
    // Don't auto-start audio - wait for user to click play
    std::cout << "🎧 Synthesizer ready! Click Play to start audio." << std::endl;
}

SynthesizerWindow::~SynthesizerWindow() {
    audioEngine.stop();
}

// -----------------------------------------------------------------------------
// UI Setup
// -----------------------------------------------------------------------------
void SynthesizerWindow::setupUI() {
    setWindowTitle("🎛️ Modular Synthesizer");
    setMinimumSize(850, 600);
    
    mainWidget = new QWidget();
    mainLayout = new QVBoxLayout(mainWidget);
    setCentralWidget(mainWidget);
    
    // Preset selector
    QHBoxLayout* presetLayout = new QHBoxLayout();
    presetLayout->addWidget(new QLabel("Preset:"));
    
    presetSelector = new QComboBox();
    for (const auto& name : presetManager.getPresetNames()) {
        presetSelector->addItem(QString::fromStdString(name));
    }
    presetSelector->setFixedWidth(200);
    presetLayout->addWidget(presetSelector);
    
    loadButton = new QPushButton("Load");
    loadButton->setFixedSize(60, 30);
    presetLayout->addWidget(loadButton);
    
    // Add play/stop buttons
    playButton = new QPushButton("▶ Play");
    playButton->setFixedSize(80, 30);
    playButton->setStyleSheet("QPushButton { background-color: #27AE60; color: white; font-weight: bold; border-radius: 4px; }");
    presetLayout->addWidget(playButton);
    
    stopButton = new QPushButton("⏹ Stop");
    stopButton->setFixedSize(80, 30);
    stopButton->setStyleSheet("QPushButton { background-color: #E74C3C; color: white; font-weight: bold; border-radius: 4px; }");
    stopButton->setEnabled(false);
    presetLayout->addWidget(stopButton);
    
    presetLayout->addStretch();
    
    mainLayout->addLayout(presetLayout);
    
    // Separator
    QLabel* separator = new QLabel("");
    separator->setFixedHeight(10);
    separator->setStyleSheet("border-bottom: 2px solid #ccc;");
    mainLayout->addWidget(separator);
    
    // Controls label
    controlsLabel = new QLabel("🎛️ PARAMETER CONTROLS");
    controlsLabel->setStyleSheet("font-weight: bold; font-size: 16px; padding: 10px;");
    controlsLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(controlsLabel);
    
    // Scrollable area
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    
    scrollContent = new QWidget();
    scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollLayout->setSpacing(5);
    scrollLayout->setContentsMargins(5, 5, 5, 5);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);
    
    // Connections
    connect(loadButton, &QPushButton::clicked, this, &SynthesizerWindow::onLoadPreset);
    connect(playButton, &QPushButton::clicked, this, &SynthesizerWindow::onPlay);
    connect(stopButton, &QPushButton::clicked, this, &SynthesizerWindow::onStop);
}

// -----------------------------------------------------------------------------
// Audio Setup
// -----------------------------------------------------------------------------
void SynthesizerWindow::setupAudio() {
    audioSyncTimer = new QTimer(this);
    connect(audioSyncTimer, &QTimer::timeout, this, &SynthesizerWindow::syncAudioParameters);
    audioSyncTimer->start(50);
}

// -----------------------------------------------------------------------------
// Preset Handling
// -----------------------------------------------------------------------------
void SynthesizerWindow::onLoadPreset() {
    int index = presetSelector->currentIndex();
    presetManager.loadPreset(index, audioEngine.getSound(), controller);
    
    clearDynamicControls();
    createParameterControls();
}

// -----------------------------------------------------------------------------
// UI Cleanup
// -----------------------------------------------------------------------------
void SynthesizerWindow::clearDynamicControls() {
    for (auto* layout : parameterLayouts) {
        while (QLayoutItem* item = layout->takeAt(0)) {
            if (QWidget* widget = item->widget()) widget->deleteLater();
            delete item;
        }
        delete layout;
    }
    parameterLayouts.clear();
    controlButtons.clear();

    while (QLayoutItem* item = scrollLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        else delete item;
    }
}

// -----------------------------------------------------------------------------
// Create Parameter Controls (Hierarchical)
// -----------------------------------------------------------------------------
void SynthesizerWindow::createParameterControls() {
    std::map<std::string, std::vector<int>> parameterGroups;
    std::vector<std::string> prefixes;

    // Collect prefixes
    for (int i = 0; i < controller.getParameterCount(); ++i) {
        const auto& param = controller.getParameter(i);
        std::string paramName = param.name;

        size_t pos = 0;
        while ((pos = paramName.find(" ", pos)) != std::string::npos) {
            std::string prefix = paramName.substr(0, pos);
            if (std::find(prefixes.begin(), prefixes.end(), prefix) == prefixes.end())
                prefixes.push_back(prefix);
            pos++;
        }
    }

    std::sort(prefixes.begin(), prefixes.end(),
        [](const std::string& a, const std::string& b) {
            return a.length() > b.length();
        });

    // Group parameters
    for (int i = 0; i < controller.getParameterCount(); ++i) {
        const auto& param = controller.getParameter(i);
        std::string paramName = param.name;

        if (paramName == "Master Volume") {
            parameterGroups["Master Controls"].push_back(i);
            continue;
        }

        bool assigned = false;
        for (const auto& prefix : prefixes) {
            if (paramName.rfind(prefix, 0) == 0) {
                parameterGroups[prefix].push_back(i);
                assigned = true;
                break;
            }
        }
        if (!assigned)
            parameterGroups["Main"].push_back(i);
    }

    // Build hierarchy
    std::map<std::string, std::map<std::string, std::vector<int>>> hierarchicalGroups;
    for (const auto& entry : parameterGroups) {
        std::string prefix = entry.first;
        std::vector<int> indices = entry.second;

        std::istringstream iss(prefix);
        std::vector<std::string> parts;
        std::string part;
        while (std::getline(iss, part, ' '))
            if (!part.empty()) parts.push_back(part);

        std::string path;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) path += " → ";
            path += parts[i];
        }

        std::string topLevel = parts.empty() ? "Main" : parts[0];
        hierarchicalGroups[topLevel][path] = indices;
    }

    // Create controls
    for (const auto& topLevel : hierarchicalGroups) {
        std::string topLevelName = topLevel.first;
        
        QLabel* topLevelHeader = new QLabel(QString::fromStdString("📁 " + topLevelName));
        topLevelHeader->setStyleSheet(
            "font-weight: bold; font-size: 16px; background-color: #E0E7FF;"
            "border-left: 5px solid #3498DB; padding: 10px; margin-top: 10px;"
        );
        scrollLayout->addWidget(topLevelHeader);
        
        for (const auto& subgroup : topLevel.second) {
            std::string path = subgroup.first;
            const auto& indices = subgroup.second;

            if (path != topLevelName)
                createParameterGroup(path, indices);
            else
                for (int paramIndex : indices)
                    createSingleParameter(paramIndex, 1);
        }

        QLabel* spacer = new QLabel("");
        spacer->setFixedHeight(15);
        scrollLayout->addWidget(spacer);
    }
}

// -----------------------------------------------------------------------------
// Create Group Header
// -----------------------------------------------------------------------------
void SynthesizerWindow::createParameterGroup(const std::string& groupName, const std::vector<int>& paramIndices) {
    if (paramIndices.empty()) return;
    
    int indentLevel = 0;
    std::string arrow = "→";
    size_t pos = 0;
    while ((pos = groupName.find(arrow, pos)) != std::string::npos) {
        indentLevel++;
        pos += arrow.length();
    }
    
    QHBoxLayout* headerLayout = new QHBoxLayout();
    for (int i = 0; i < indentLevel; ++i)
        headerLayout->addWidget(new QLabel("    "));
    
    QLabel* groupLabel = new QLabel(QString::fromStdString("📁 " + groupName));
    QString headerStyle = "font-weight: bold; font-size: 14px; color: #2C3E50; padding: 8px 0px;";
    
    if (indentLevel == 0)
        headerStyle += "background-color: #E8F4FD; border-left: 4px solid #3498DB;";
    else if (indentLevel == 1)
        headerStyle += "background-color: #FFF2E8; border-left: 4px solid #E67E22;";
    else if (indentLevel == 2)
        headerStyle += "background-color: #E8F8E8; border-left: 4px solid #27AE60;";
    
    groupLabel->setStyleSheet(headerStyle);
    headerLayout->addWidget(groupLabel);
    headerLayout->addStretch();
    
    scrollLayout->addLayout(headerLayout);
    parameterLayouts.push_back(headerLayout);
    
    for (int paramIndex : paramIndices)
        createSingleParameter(paramIndex, indentLevel + 1);
    
    QLabel* spacer = new QLabel("");
    spacer->setFixedHeight(10);
    scrollLayout->addWidget(spacer);
}

// -----------------------------------------------------------------------------
// Create Parameter Row
// -----------------------------------------------------------------------------
void SynthesizerWindow::createSingleParameter(int paramIndex, int indentLevel) {
    const auto param = controller.getParameter(paramIndex);
    
    // Check if this is a volume parameter
    bool isVolumeParam = (param.name.find("Volume") != std::string::npos) || 
                        (param.name.find("volume") != std::string::npos);
    
    QHBoxLayout* paramLayout = new QHBoxLayout();
    QString indentStr;
    for (int i = 0; i < indentLevel; ++i) indentStr += "    ";

    std::string displayName = param.name;
    size_t lastSpace = displayName.find_last_of(' ');
    if (lastSpace != std::string::npos)
        displayName = displayName.substr(lastSpace + 1);

    QLabel* label = new QLabel(indentStr + "🔧 " + QString::fromStdString(displayName) + ":");
    label->setFixedWidth(180 + (indentLevel * 20));
    QString labelStyle = "font-weight: bold; padding: 2px;";
    if (indentLevel == 1) labelStyle += "color: #2980B9;";
    else if (indentLevel == 2) labelStyle += "color: #D35400;";
    else if (indentLevel == 3) labelStyle += "color: #27AE60;";
    label->setStyleSheet(labelStyle);
    paramLayout->addWidget(label);

    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 1000);

    double minV = param.minValue;
    double maxV = param.maxValue;
    double curV = (param.valuePtr ? *param.valuePtr : minV);
    double range = maxV - minV;
    double normalized = (range > 0.0) ? ((curV - minV) / range) : 0.0;
    int initialSliderValue = std::clamp<int>(static_cast<int>(std::round(normalized * 1000.0)), 0, 1000);
    slider->setValue(initialSliderValue);

    slider->setStyleSheet(
        "QSlider::groove:horizontal { height: 8px; background: #E0E0E0; } "
        "QSlider::handle:horizontal { background: #3498DB; width: 18px; margin: -5px 0; border-radius: 9px; }"
    );
    slider->setFixedWidth(150);
    paramLayout->addWidget(slider);

    QLineEdit* valueEdit = new QLineEdit(QString::number(static_cast<int>(std::round(curV))));
    valueEdit->setFixedWidth(70);
    valueEdit->setAlignment(Qt::AlignCenter);
    valueEdit->setStyleSheet("border: 2px solid #BDC3C7; padding: 4px; background: white; border-radius: 4px;");
    QIntValidator* validator = new QIntValidator(static_cast<int>(minV), static_cast<int>(maxV), valueEdit);
    valueEdit->setValidator(validator);
    paramLayout->addWidget(valueEdit);

    QLabel* rangeLabel = new QLabel(QString("(%1 - %2)")
                                   .arg(static_cast<int>(minV))
                                   .arg(static_cast<int>(maxV)));
    rangeLabel->setStyleSheet("color: #7F8C8D; font-size: 9px; font-style: italic;");
    paramLayout->addWidget(rangeLabel);
    paramLayout->addStretch();

    // Signal connections with volume conversion
    connect(slider, &QSlider::valueChanged, this, [this, paramIndex, slider, valueEdit, minV, maxV, isVolumeParam]() {
        double normalizedValue = slider->value() / 1000.0;
        double actualValue = (maxV > minV)
            ? minV + normalizedValue * (maxV - minV)
            : minV;
        int intValue = static_cast<int>(std::round(actualValue));

        if (isVolumeParam) {
            // Convert 0-100 to 0.0-1.0 for audio engine
            double audioValue = intValue / 100.0;
            controller.setParameter(paramIndex, audioValue);
        } else {
            controller.setParameter(paramIndex, intValue);
        }
        valueEdit->setText(QString::number(intValue));
    });

    connect(valueEdit, &QLineEdit::editingFinished, this, [this, paramIndex, valueEdit, slider, minV, maxV, isVolumeParam]() {
        int value = valueEdit->text().toInt();
        value = std::clamp(value, static_cast<int>(minV), static_cast<int>(maxV));
        
        if (isVolumeParam) {
            // Convert 0-100 to 0.0-1.0 for audio engine
            double audioValue = value / 100.0;
            controller.setParameter(paramIndex, audioValue);
        } else {
            controller.setParameter(paramIndex, value);
        }

        double normalizedValue = (maxV > minV) ? ((value - minV) / (maxV - minV)) : 0.0;
        int sliderPos = std::clamp<int>(static_cast<int>(std::round(normalizedValue * 1000.0)), 0, 1000);

        bool oldBlock = slider->blockSignals(true);
        slider->setValue(sliderPos);
        slider->blockSignals(oldBlock);
    });

    scrollLayout->addLayout(paramLayout);
    parameterLayouts.push_back(paramLayout);
}

// -----------------------------------------------------------------------------
// Sync Audio Parameters
// -----------------------------------------------------------------------------
void SynthesizerWindow::syncAudioParameters() {
    // Placeholder for additional real-time sync logic if needed.
}

void SynthesizerWindow::onPlay() {
    if (!isPlaying) {
        audioEngine.start();
        isPlaying = true;
        playButton->setEnabled(false);
        stopButton->setEnabled(true);
        std::cout << "🎵 Audio started" << std::endl;
    }
}

void SynthesizerWindow::onStop() {
    if (isPlaying) {
        audioEngine.stop();
        isPlaying = false;
        playButton->setEnabled(true);
        stopButton->setEnabled(false);
        std::cout << "⏸️ Audio stopped" << std::endl;
    }
}