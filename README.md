# MIDI Clock LED Comet Visualizer

A responsive, high-performance C++ firmware for the ESP32 that visualizes MIDI Clock input on a WS2812B (NeoPixel) LED strip. It generates comet-like pulses traveling down the strip in sync with the musical tempo.

---

## Features

*   **MIDI Clock Sync**: Synchronizes animation directly with standard MIDI clock ticks (24 PPQ).
*   **Ultra-Smooth Motion**: Uses sub-tick time interpolation (with exponential moving average filtering) to continuously compute fractional comet positions between physical MIDI clocks, rendering at a smooth 60 FPS.
*   **Beat & Measure Progression**: Launches a new comet every 16th note (every 6 clocks). The downbeat of each measure (Beat 1, Tick 0) launches a visually distinct comet (pure white or custom color) to clearly demarcate the musical timeline.
*   **Additive Blending**: Overlapping comets blend colors additively, naturally highlighting denser portions of the music without flickering.
*   **Note Activity-Weighted Coloring**: Snaps the instrumentation state (active channels) at the moment a comet is launched to blend its color. Active instruments leave a physical history along the strip as the comets travel.
*   **Advanced Visual Adjustments**:
    *   *Boundary Easing*: Smoothly fades comets in and out near the strip edges.
    *   *Gamma Correction*: Employs FastLED's optimized `dim8_video` for natural brightness falloff.
    *   *Direction Toggling*: Reversable travel direction (forward or backward).
    *   *Custom Speed*: Configurable travel length of 1 or 2 bars.
*   **Diagnostic Debug Mode**: Serial console logging of MIDI clock connection and 16th-note boundaries for easy debugging.

---

## Project Directory Structure

*   [src/main.cpp](file:///Users/james/projects/midi_comet/src/main.cpp): Core visualizer firmware, MIDI callback registrations, time interpolation, and LED rendering loops.
*   [include/constants.h](file:///Users/james/projects/midi_comet/include/constants.h): User configuration file. Defines pins, LED strip counts, colors, decay rates, and toggles for stretch goal features.
*   [platformio.ini](file:///Users/james/projects/midi_comet/platformio.ini): Build descriptor specifying the PlatformIO environment (`esp32dev`) and dependency libraries (`FastLED`, `MIDI Library`).
*   [HARDWARE.md](file:///Users/james/projects/midi_comet/HARDWARE.md): Comprehensive wiring guide containing step-by-step breadboarding instructions and schematics for both DIY optocoupler and pre-built module circuits.

---

## Quick Start

### 1. Hardware Assembly
Assemble your circuit on a breadboard. Refer to [HARDWARE.md](file:///Users/james/projects/midi_comet/HARDWARE.md) for step-by-step guides.
- Connect your **WS2812B LED strip** to **GPIO 13** (via a 330 $\Omega$ resistor).
- Wire up a **6N137 optocoupler** to isolate your MIDI DIN-5 signal and route the output to **GPIO 16 (RX2)**.
- Connect the ESP32 to your PC using a USB cable.

### 2. Configuration
Open [include/constants.h](file:///Users/james/projects/midi_comet/include/constants.h) and verify or adjust the settings to match your setup:
*   `NUM_LEDS`: Total number of addressable LEDs on your strip.
*   `LED_PIN` and `MIDI_RX_PIN`: Update if you used different GPIO pins.
*   `DEBUG_MODE`: Uncomment/comment out to enable or disable USB Serial monitoring logs.

### 3. Build & Upload
Open a terminal in the project directory `/Users/james/projects/midi_comet/` and execute the PlatformIO commands:

To build the project:
```bash
/Users/james/.platformio/penv/bin/pio run
```

To upload the compiled binary to your connected ESP32:
```bash
/Users/james/.platformio/penv/bin/pio run -t upload
```

### 4. Play Music
Connect your MIDI source (DAW, sequencer, or hardware controller) sending MIDI clock and notes. Watch the strip ignite in sync with your tracks.
If `DEBUG_MODE` is enabled, open the Serial Monitor at **115200 baud** to view real-time timing reports:
```text
MIDI Clock LED Comet Visualizer Initiated.
[DEBUG] First MIDI Clock tick detected!
[DEBUG] 16th Note. Clock counter: 6. Est BPM: 120.0
[DEBUG] 16th Note. Clock counter: 12. Est BPM: 120.0
...
```
