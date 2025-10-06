# Qt Audio Synthesizer

A simple Qt-based audio synthesizer project that generates sine waves in real-time.

## Project Structure

- **Sound.h**: Abstract base class defining the interface for all sound generators
- **SineWave.h/.cpp**: Concrete implementation of a sine wave generator
- **AudioEngine.cpp**: Handles Qt audio output and buffer management
- **main.cpp**: Application entry point and UI setup

## Key Design Principles

1. **Interface Segregation**: Sound.h provides a clean interface that any audio generator can implement
2. **Stable Audio**: SineWave uses proper phase management and amplitude limiting to prevent clipping
3. **Extensible**: Easy to add new sound types (square wave, sawtooth, etc.) by implementing Sound interface

## Audio Details

- Sample rate: Configurable via Qt's audio system
- Amplitude: Limited to 0.1 (10%) to prevent ear damage and clipping
- Phase management: Proper wraparound to avoid numerical drift

## Building

```bash
make clean && make
./myqtapp
```

## Next Steps

- Add more waveform types (square, sawtooth, triangle)
- Implement ADSR envelope
- Add frequency modulation
- Create a proper GUI with sliders
