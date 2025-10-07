# Modular Sound Synthesis System - Qt6 Only (Single AudioEngine)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.

# Qt6 setup - Use correct MOC path we found
QT6_PATH = /opt/homebrew/opt/qt
QT6_CELLAR = /opt/homebrew/Cellar/qt/6.9.2
QT6_INCLUDES = -I$(QT6_PATH)/include -I$(QT6_PATH)/include/QtCore -I$(QT6_PATH)/include/QtMultimedia -I$(QT6_PATH)/include/QtWidgets -I$(QT6_PATH)/include/QtGui
QT6_LIBS = -F$(QT6_PATH)/lib -framework QtCore -framework QtGui -framework QtMultimedia -framework QtWidgets
MOC = $(QT6_CELLAR)/share/qt/libexec/moc

# Source directories - add filters directory
SRC_DIRS = . core oscillators synthesizers audio interface presets gui filters envelopes

# TEMPORARILY include legacy files until we finish the transition
ALL_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
# SOURCES = $(filter-out core/SoundInterface.cpp core/SoundPreset.cpp presets/PresetLibrary.cpp interface/SimpleInterface.cpp gui/ParameterControlWidget.cpp, $(ALL_SOURCES))
SOURCES = $(ALL_SOURCES)
OBJECTS = $(SOURCES:.cpp=.o)

# MOC files - only SynthesizerWindow and AudioEngine
MOC_HEADERS = audio/AudioEngine.h gui/SynthesizerWindow.h
MOC_SOURCES = $(MOC_HEADERS:.h=_moc.cpp)
MOC_OBJECTS = $(MOC_SOURCES:.cpp=.o)

TARGET = synth_live

# IMPORTANT: The all target must be the first target defined!
# Default target - live audio with Qt6
all: $(TARGET)

$(TARGET): $(OBJECTS) $(MOC_OBJECTS)
	$(CXX) $(OBJECTS) $(MOC_OBJECTS) $(QT6_LIBS) -o $(TARGET)

# Compile .cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(QT6_INCLUDES) -c $< -o $@

# Generate MOC files - only the ones we need
audio/AudioEngine_moc.cpp: audio/AudioEngine.h
	$(MOC) $< -o $@

gui/SynthesizerWindow_moc.cpp: gui/SynthesizerWindow.h
	$(MOC) $< -o $@

audio/AudioEngine_moc.o: audio/AudioEngine_moc.cpp
	$(CXX) $(CXXFLAGS) $(QT6_INCLUDES) -c $< -o $@

gui/SynthesizerWindow_moc.o: gui/SynthesizerWindow_moc.cpp
	$(CXX) $(CXXFLAGS) $(QT6_INCLUDES) -c $< -o $@

# Add dependency for SynthesizerWindow
gui/SynthesizerWindow.o: gui/SynthesizerWindow.cpp gui/SynthesizerWindow_moc.cpp
	$(CXX) $(CXXFLAGS) $(QT6_INCLUDES) -c gui/SynthesizerWindow.cpp -o gui/SynthesizerWindow.o

# Clean everything
clean:
	rm -f $(foreach dir,$(SRC_DIRS),$(dir)/*.o) $(MOC_SOURCES) $(TARGET)
	rm -f gui/ParameterControlWidget_moc.*  # Clean up any remaining old files

# Add a clean target for removing legacy files completely
clean-legacy:
	rm -f core/SoundInterface.* core/SoundPreset.* presets/PresetLibrary.* interface/SimpleInterface.*

# Rebuild everything
rebuild: clean all

# Debug: Let's check what's available (optional)
debug-qt:
	@echo "Checking Qt6 installation..."
	@find /opt/homebrew/Cellar/qt/ -name "moc" -type f 2>/dev/null || echo "MOC not found"

# Debug library paths
debug-libs:
	@echo "Checking Qt6 libraries..."
	@ls -la $(QT6_PATH)/lib/ | head -10
	@echo "Looking for Qt6 frameworks:"
	@find $(QT6_PATH)/lib -name "*Qt*" -type d 2>/dev/null | head -5

.PHONY: all clean rebuild debug-qt debug-libs clean-legacy

SRCS = $(wildcard main.cpp core/*.cpp oscillators/*.cpp synthesizers/*.cpp filters/*.cpp envelopes/*.cpp presets/*.cpp)