# CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project(QtSynth)

set(CMAKE_CXX_STANDARD 14)

# Find QtWidgets library
find_package(Qt5Widgets REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizers
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

# Source files
set(SOURCES
    main.cpp
    audio/AudioEngine.cpp
    presets/PresetManager.cpp
    oscillators/SineOscillator.cpp
    oscillators/SawOscillator.cpp
    synthesizers/FMSynthesizer.cpp
    gui/SynthesizerWindow.cpp
)

# Executable
add_executable(QtSynth ${SOURCES})

# Link QtWidgets library
target_link_libraries(QtSynth Qt5::Widgets)