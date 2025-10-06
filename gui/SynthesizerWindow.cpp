#include "SynthesizerWindow.h"
#include <QApplication>
#include <iostream>

SynthesizerWindow::SynthesizerWindow(QWidget* parent)
    : QMainWindow(parent), mainWidget(nullptr), mainLayout(nullptr) {
    
    setupUI();
    setupAudio();
    
    // Load default preset
    presetManager.loadPreset(0, audioEngine.getSound(), controller);
    createParameterControls();
    
    // Start audio
    audioEngine.start();
    std::cout << "🎧 Synthesizer ready!" << std::endl;
}

SynthesizerWindow::~SynthesizerWindow() {
    audioEngine.stop();
}

void SynthesizerWindow::setupUI() {
    setWindowTitle("🎛️ Modular Synthesizer");
    setFixedSize(750, 650);
    
    mainWidget = new QWidget();
    mainLayout = new QVBoxLayout(mainWidget);
    setCentralWidget(mainWidget);
    
    // Parameter display
    parameterDisplay = new QTextEdit();
    parameterDisplay->setReadOnly(true);
    parameterDisplay->setFixedHeight(150);
    parameterDisplay->setStyleSheet(
        "background-color: #2b2b2b; color: #ffffff; "
        "font-family: 'Courier New', monospace; font-size: 11px; padding: 8px;"
    );
    mainLayout->addWidget(parameterDisplay);
    
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
    
    mainLayout->addStretch();
    
    // Connect signals
    connect(loadButton, &QPushButton::clicked, this, &SynthesizerWindow::onLoadPreset);
}

void SynthesizerWindow::setupAudio() {
    // Setup timers
    displayUpdateTimer = new QTimer(this);
    connect(displayUpdateTimer, &QTimer::timeout, this, &SynthesizerWindow::updateDisplay);
    displayUpdateTimer->start(100);
    
    audioSyncTimer = new QTimer(this);
    connect(audioSyncTimer, &QTimer::timeout, this, &SynthesizerWindow::syncAudioParameters);
    audioSyncTimer->start(50);
}

void SynthesizerWindow::onLoadPreset() {
    int index = presetSelector->currentIndex();
    presetManager.loadPreset(index, audioEngine.getSound(), controller);
    
    clearDynamicControls();
    createParameterControls();
}

void SynthesizerWindow::clearDynamicControls() {
    for (auto* layout : parameterLayouts) {
        while (QLayoutItem* item = layout->takeAt(0)) {
            if (QWidget* widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
        mainLayout->removeItem(layout);
        delete layout;
    }
    parameterLayouts.clear();
    controlButtons.clear();
}

void SynthesizerWindow::createParameterControls() {
    // Parse parameter names to create hierarchical structure
    std::map<std::string, std::vector<int>> parameterGroups;
    
    // Group parameters by their prefix hierarchy
    for (int i = 0; i < controller.getParameterCount(); ++i) {
        const auto& param = controller.getParameter(i);
        std::string paramName = param.name;
        
        // Extract the base component name (e.g., "FM", "FM Carrier", "FM Carrier Carrier")
        std::string groupKey = "Main";
        if (paramName.find("FM Carrier Carrier") != std::string::npos) {
            groupKey = "FM → Carrier → Nested FM";
        } else if (paramName.find("FM Carrier") != std::string::npos) {
            groupKey = "FM → Carrier";
        } else if (paramName.find("FM Modulator") != std::string::npos) {
            groupKey = "FM → Modulator";
        } else if (paramName.find("FM") != std::string::npos) {
            groupKey = "FM → Main";
        } else if (paramName.find("Sine") != std::string::npos) {
            groupKey = "Sine Oscillator";
        }
        
        parameterGroups[groupKey].push_back(i);
    }
    
    // Create grouped controls with clear hierarchy
    for (const auto& group : parameterGroups) {
        createParameterGroup(group.first, group.second);
    }
}

void SynthesizerWindow::createParameterGroup(const std::string& groupName, const std::vector<int>& paramIndices) {
    if (paramIndices.empty()) return;
    
    // Determine indentation level based on hierarchy
    int indentLevel = 0;
    std::string arrow = "→";
    size_t pos = 0;
    while ((pos = groupName.find(arrow, pos)) != std::string::npos) {
        indentLevel++;
        pos += arrow.length();
    }
    
    // Create group header
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    // Add indentation
    for (int i = 0; i < indentLevel; ++i) {
        QLabel* indent = new QLabel("    ");  // 4 spaces per level
        headerLayout->addWidget(indent);
    }
    
    // Group title with hierarchy styling
    QLabel* groupLabel = new QLabel(QString::fromStdString("📁 " + groupName));
    QString headerStyle = "font-weight: bold; font-size: 14px; color: #2C3E50; padding: 8px 0px;";
    
    // Different colors for different levels
    if (indentLevel == 0) {
        headerStyle += "background-color: #E8F4FD; border-left: 4px solid #3498DB;";
    } else if (indentLevel == 1) {
        headerStyle += "background-color: #FFF2E8; border-left: 4px solid #E67E22;";
    } else if (indentLevel == 2) {
        headerStyle += "background-color: #E8F8E8; border-left: 4px solid #27AE60;";
    }
    
    groupLabel->setStyleSheet(headerStyle);
    headerLayout->addWidget(groupLabel);
    headerLayout->addStretch();
    
    mainLayout->insertLayout(mainLayout->count() - 1, headerLayout);
    parameterLayouts.push_back(headerLayout);
    
    // Create parameters for this group
    for (int paramIndex : paramIndices) {
        createSingleParameter(paramIndex, indentLevel + 1);
    }
    
    // Add spacing after group
    QLabel* spacer = new QLabel("");
    spacer->setFixedHeight(10);
    mainLayout->insertWidget(mainLayout->count() - 1, spacer);
}

void SynthesizerWindow::createSingleParameter(int paramIndex, int indentLevel) {
    const auto& param = controller.getParameter(paramIndex);
    
    QHBoxLayout* paramLayout = new QHBoxLayout();
    
    // Add hierarchical indentation
    QString indentStr = "";
    for (int i = 0; i < indentLevel; ++i) {
        indentStr += "    ";  // 4 spaces per level
    }
    
    // Extract just the parameter name (remove prefix)
    std::string displayName = param.name;
    size_t lastSpace = displayName.find_last_of(' ');
    if (lastSpace != std::string::npos) {
        displayName = displayName.substr(lastSpace + 1);
    }
    
    // Parameter label with indentation
    QLabel* label = new QLabel(indentStr + "🔧 " + QString::fromStdString(displayName) + ":");
    label->setFixedWidth(200 + (indentLevel * 20));  // Wider for indented items
    
    // Styling based on hierarchy level
    QString labelStyle = "font-weight: bold; padding: 2px;";
    if (indentLevel == 1) {
        labelStyle += "color: #2980B9;";  // Blue for level 1
    } else if (indentLevel == 2) {
        labelStyle += "color: #D35400;";  // Orange for level 2  
    } else if (indentLevel == 3) {
        labelStyle += "color: #27AE60;";  // Green for level 3
    }
    label->setStyleSheet(labelStyle);
    paramLayout->addWidget(label);
    
    // Decrease button
    QPushButton* decBtn = new QPushButton("−");
    decBtn->setFixedSize(25, 25);
    decBtn->setStyleSheet("font-weight: bold; font-size: 14px; background-color: #E74C3C; color: white; border-radius: 12px;");
    paramLayout->addWidget(decBtn);
    
    // Value display
    QLabel* valueLabel = new QLabel(QString::number(*param.valuePtr, 'f', 1));
    valueLabel->setFixedWidth(70);
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setStyleSheet("border: 2px solid #BDC3C7; padding: 4px; background: white; border-radius: 4px;");
    paramLayout->addWidget(valueLabel);
    
    // Increase button
    QPushButton* incBtn = new QPushButton("+");
    incBtn->setFixedSize(25, 25);
    incBtn->setStyleSheet("font-weight: bold; font-size: 14px; background-color: #27AE60; color: white; border-radius: 12px;");
    paramLayout->addWidget(incBtn);
    
    // Range info with smaller, subtle styling
    QLabel* rangeLabel = new QLabel(QString("(%1 - %2)")
                                   .arg(param.minValue, 0, 'f', 1)
                                   .arg(param.maxValue, 0, 'f', 1));
    rangeLabel->setStyleSheet("color: #7F8C8D; font-size: 9px; font-style: italic;");
    paramLayout->addWidget(rangeLabel);
    paramLayout->addStretch();
    
    // Connect buttons
    connect(decBtn, &QPushButton::clicked, [this, paramIndex, valueLabel, &param]() {
        controller.decreaseParameter(paramIndex);
        valueLabel->setText(QString::number(*param.valuePtr, 'f', 1));
    });
    
    connect(incBtn, &QPushButton::clicked, [this, paramIndex, valueLabel, &param]() {
        controller.increaseParameter(paramIndex);
        valueLabel->setText(QString::number(*param.valuePtr, 'f', 1));
    });
    
    mainLayout->insertLayout(mainLayout->count() - 1, paramLayout);
    parameterLayouts.push_back(paramLayout);
    controlButtons.push_back(decBtn);
    controlButtons.push_back(incBtn);
}

void SynthesizerWindow::updateDisplay() {
    QString text = "🎛️ MODULAR SYNTHESIZER - HIERARCHICAL VIEW\n";
    text += QString("═").repeated(50) + "\n\n";
    
    // Group parameters by hierarchy for display
    std::map<std::string, std::vector<int>> groups;
    for (int i = 0; i < controller.getParameterCount(); ++i) {
        const auto& param = controller.getParameter(i);
        std::string paramName = param.name;
        
        std::string groupKey = "Main";
        if (paramName.find("FM Carrier Carrier") != std::string::npos) {
            groupKey = "  └─ Carrier (Nested FM)";
        } else if (paramName.find("FM Carrier") != std::string::npos) {
            groupKey = "├─ Carrier";
        } else if (paramName.find("FM Modulator") != std::string::npos) {
            groupKey = "└─ Modulator";
        } else if (paramName.find("FM") != std::string::npos) {
            groupKey = "FM Synthesizer";
        }
        
        groups[groupKey].push_back(i);
    }
    
    for (const auto& group : groups) {
        text += QString::fromStdString(group.first) + "\n";
        for (int idx : group.second) {
            const auto& param = controller.getParameter(idx);
            
            // Extract parameter name
            std::string displayName = param.name;
            size_t lastSpace = displayName.find_last_of(' ');
            if (lastSpace != std::string::npos) {
                displayName = displayName.substr(lastSpace + 1);
            }
            
            text += QString("  │ %1: %2\n")
                   .arg(QString::fromStdString(displayName).leftJustified(12))
                   .arg(*param.valuePtr, 6, 'f', 1);
        }
        text += "\n";
    }
    
    text += "🎧 Status: PLAYING | Structure: NESTED";
    parameterDisplay->setText(text);
}

void SynthesizerWindow::syncAudioParameters() {
    // Audio parameter syncing is now handled by the parameter callbacks
    // This method can be used for additional real-time updates if needed
}
