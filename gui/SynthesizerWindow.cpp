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
    setMinimumSize(850, 700); // Increased width to accommodate sliders
    
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
    
    // Create scrollable area for parameters
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    
    // Create content widget for the scroll area
    scrollContent = new QWidget();
    scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollLayout->setSpacing(5);
    scrollLayout->setContentsMargins(5, 5, 5, 5);
    
    // Add the content widget to scroll area
    scrollArea->setWidget(scrollContent);
    
    // Add scroll area to main layout
    mainLayout->addWidget(scrollArea, 1); // 1 = stretch factor to take available space
    
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
        // Do not remove from mainLayout anymore as we're using scrollLayout
        delete layout;
    }
    parameterLayouts.clear();
    controlButtons.clear();
    
    // Clear all widgets from scroll content layout
    while (QLayoutItem* item = scrollLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
}

void SynthesizerWindow::createParameterControls() {
    // Parse parameter names to create hierarchical structure
    std::map<std::string, std::vector<int>> parameterGroups;
    
    // Group parameters by their prefix hierarchy
    for (int i = 0; i < controller.getParameterCount(); ++i) {
        const auto& param = controller.getParameter(i);
        std::string paramName = param.name;
        
        // Extract the base component name with improved hierarchy detection
        std::string groupKey = "Main";
        
        // Master volume always goes in its own group
        if (paramName.find("Master Volume") != std::string::npos) {
            groupKey = "Master Controls";
        }
        // Handle nested FM structure with improved detection logic
        else if (paramName.find("FM Carrier Carrier") != std::string::npos) {
            // Deeper nested FM structure
            groupKey = "FM → Carrier → Nested FM";
        } 
        else if (paramName.find("FM Carrier Modulator") != std::string::npos) {
            groupKey = "FM → Carrier → Modulator";
        }
        else if (paramName.find("FM Carrier") != std::string::npos) {
            // Check if this is a Sine oscillator parameter
            if (paramName.find("Sine") != std::string::npos) {
                groupKey = "FM → Carrier → Sine Oscillator";
            } else {
                groupKey = "FM → Carrier";
            }
        }
        else if (paramName.find("FM Modulator") != std::string::npos) {
            // Check if this is a Sine oscillator parameter
            if (paramName.find("Sine") != std::string::npos) {
                groupKey = "FM → Modulator → Sine Oscillator";
            } else {
                groupKey = "FM → Modulator";
            }
        }
        else if (paramName.find("FM") != std::string::npos) {
            // Main FM parameters
            groupKey = "FM Synthesizer";
        }
        else if (paramName.find("Sine") != std::string::npos) {
            // Top-level sine oscillator
            groupKey = "Sine Oscillator";
        }
        
        parameterGroups[groupKey].push_back(i);
    }
    
    // Create grouped controls with clear hierarchy
    // Sort the keys to ensure consistent ordering
    std::vector<std::string> sortedKeys;
    for (const auto& entry : parameterGroups) {
        sortedKeys.push_back(entry.first);
    }
    
    // Custom sort to ensure proper hierarchical display order
    std::sort(sortedKeys.begin(), sortedKeys.end(), [](const std::string& a, const std::string& b) {
        // Custom sorting logic for hierarchical display
        
        // Master Controls should always be last
        if (a == "Master Controls") return false;
        if (b == "Master Controls") return true;
        
        // Main categories come first
        bool aIsMain = (a == "Sine Oscillator" || a == "FM Synthesizer");
        bool bIsMain = (b == "Sine Oscillator" || b == "FM Synthesizer");
        if (aIsMain && !bIsMain) return true;
        if (!aIsMain && bIsMain) return false;
        
        // Sort by nesting depth (more arrows = deeper nesting)
        // Fix: Count occurrences of the arrow string instead of character
        size_t aDepth = 0;
        size_t pos = 0;
        std::string arrow = "→";
        while ((pos = a.find(arrow, pos)) != std::string::npos) {
            aDepth++;
            pos += arrow.length();
        }
        
        size_t bDepth = 0;
        pos = 0;
        while ((pos = b.find(arrow, pos)) != std::string::npos) {
            bDepth++;
            pos += arrow.length();
        }
        
        if (aDepth != bDepth) return aDepth < bDepth;
        
        // Finally sort alphabetically
        return a < b;
    });
    
    for (const auto& key : sortedKeys) {
        createParameterGroup(key, parameterGroups[key]);
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
    
    // Add to scroll layout instead of main layout
    scrollLayout->addLayout(headerLayout);
    parameterLayouts.push_back(headerLayout);
    
    // Create parameters for this group
    for (int paramIndex : paramIndices) {
        createSingleParameter(paramIndex, indentLevel + 1);
    }
    
    // Add spacing after group
    QLabel* spacer = new QLabel("");
    spacer->setFixedHeight(10);
    scrollLayout->addWidget(spacer);
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
    label->setFixedWidth(180 + (indentLevel * 20));  // Slightly narrower to accommodate slider
    
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
    
    // Create slider for value adjustment
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 1000); // 1000 steps for fine control
    slider->setValue(((*param.valuePtr - param.minValue) / (param.maxValue - param.minValue)) * 1000);
    slider->setStyleSheet("QSlider::groove:horizontal { height: 8px; background: #E0E0E0; } "
                         "QSlider::handle:horizontal { background: #3498DB; width: 18px; margin: -5px 0; border-radius: 9px; }");
    slider->setFixedWidth(150);
    paramLayout->addWidget(slider);
    
    // Value display as an editable field
    QLineEdit* valueEdit = new QLineEdit(QString::number(*param.valuePtr, 'f', 1));
    valueEdit->setFixedWidth(70);
    valueEdit->setAlignment(Qt::AlignCenter);
    valueEdit->setStyleSheet("border: 2px solid #BDC3C7; padding: 4px; background: white; border-radius: 4px;");
    
    // Set validator for the edit field
    QDoubleValidator* validator = new QDoubleValidator(param.minValue, param.maxValue, 1, valueEdit);
    validator->setNotation(QDoubleValidator::StandardNotation);
    valueEdit->setValidator(validator);
    
    paramLayout->addWidget(valueEdit);
    
    // Range info with smaller, subtle styling
    QLabel* rangeLabel = new QLabel(QString("(%1 - %2)")
                                   .arg(param.minValue, 0, 'f', 1)
                                   .arg(param.maxValue, 0, 'f', 1));
    rangeLabel->setStyleSheet("color: #7F8C8D; font-size: 9px; font-style: italic;");
    paramLayout->addWidget(rangeLabel);
    paramLayout->addStretch();
    
    // Connect slider to update both the parameter and the line edit
    connect(slider, &QSlider::valueChanged, [this, paramIndex, valueEdit, &param, slider]() {
        double normalizedValue = slider->value() / 1000.0;
        double actualValue = param.minValue + normalizedValue * (param.maxValue - param.minValue);
        controller.setParameter(paramIndex, actualValue);
        valueEdit->setText(QString::number(actualValue, 'f', 1));
    });
    
    // Connect line edit to update both the parameter and the slider
    connect(valueEdit, &QLineEdit::editingFinished, [this, paramIndex, valueEdit, &param, slider]() {
        double value = valueEdit->text().toDouble();
        controller.setParameter(paramIndex, value);
        
        // Update slider position
        double normalizedValue = (value - param.minValue) / (param.maxValue - param.minValue);
        slider->setValue(normalizedValue * 1000);
    });
    
    // Add to scroll layout instead of main layout
    scrollLayout->addLayout(paramLayout);
    parameterLayouts.push_back(paramLayout);
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
        
        // Match grouping logic from createParameterControls
        if (paramName.find("Master Volume") != std::string::npos) {
            groupKey = "Master Controls";
        }
        else if (paramName.find("FM Carrier Carrier") != std::string::npos) {
            groupKey = "  └─ Carrier → Nested FM";
        }
        else if (paramName.find("FM Carrier Modulator") != std::string::npos) {
            groupKey = "  └─ Carrier → Modulator";
        }
        else if (paramName.find("FM Carrier") != std::string::npos) {
            if (paramName.find("Sine") != std::string::npos) {
                groupKey = "  └─ Carrier → Sine Oscillator";
            } else {
                groupKey = "├─ Carrier";
            }
        }
        else if (paramName.find("FM Modulator") != std::string::npos) {
            if (paramName.find("Sine") != std::string::npos) {
                groupKey = "  └─ Modulator → Sine Oscillator";
            } else {
                groupKey = "└─ Modulator";
            }
        }
        else if (paramName.find("FM") != std::string::npos) {
            groupKey = "FM Synthesizer";
        }
        else if (paramName.find("Sine") != std::string::npos) {
            groupKey = "Sine Oscillator";
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
