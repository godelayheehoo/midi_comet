#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <FastLED.h>

// ==========================================
// Hardware Configuration
// ==========================================

// Number of LEDs in the WS2812 strip
#define NUM_LEDS 60

// GPIO pin connected to the WS2812/NeoPixel data line
#define LED_PIN 13

// ESP32 Hardware Serial 2 pins for DIN MIDI input
#define MIDI_RX_PIN 16
#define MIDI_TX_PIN 17

// ==========================================
// Visual Configuration
// ==========================================

// Render refresh rate (Frames Per Second)
#define FPS 60

// Normal comet color (0-255 HSV Hue, 160 is deep blue)
#define BASE_HUE 160

// Downbeat (Beat 1) comet color (0-255 HSV Hue, e.g. 0 for red)
#define DOWNBEAT_HUE 0
// Saturation for downbeat color (0 means white, 255 means fully saturated)
#define DOWNBEAT_SATURATION 0 
// Whether to force the downbeat comet to pure white (ignores DOWNBEAT_HUE/SATURATION)
#define DOWNBEAT_IS_WHITE true

// Global brightness limit (0-255)
#define MAX_BRIGHTNESS 128

// Tail length of the comets in LEDs
#define COMET_LENGTH 8.0f

// ==========================================
// Debug Configuration
// ==========================================

// Enable debug messages on the Serial Monitor (e.g. MIDI Clock detection, 16th notes)
#define DEBUG_MODE

// ==========================================
// Advanced / Stretch Goal Configuration
// ==========================================

// Direction of travel:
// false = travel from index 0 to NUM_LEDS-1 (Forward)
// true  = travel from index NUM_LEDS-1 to 0 (Reverse)
#define REVERSE_DIRECTION false

// Comet travel duration in bars (4/4 measures).
// 1 = 1 bar (96 ticks) to traverse the strip
// 2 = 2 bars (192 ticks) to traverse the strip
#define COMET_TRAVEL_BARS 1

// Fade in and out comet heads at the strip boundaries (start and end)
#define FADE_EASE_IN_OUT true
// Fraction of travel (0.0 to 1.0) where fade occurs (e.g. 0.10 = 10%)
#define FADE_LIMIT_PCT 0.10f

// Apply LED gamma correction using FastLED's optimized dim8_video
#define ENABLE_GAMMA_CORRECTION true

// Enable MIDI note activity-weighted coloring for normal comets
#define ENABLE_CHANNEL_ACTIVITY_COLORING true

// Decay rate for MIDI channel activity per frame (at FPS rate)
// 0.98 means channel activity decays by 2% each frame
#define CHANNEL_ACTIVITY_DECAY 0.98f

// Color palette for the 16 MIDI channels (used if ENABLE_CHANNEL_ACTIVITY_COLORING is true)
// You can customize the colors mapped to each channel (index 0 corresponds to MIDI Channel 1)
const CRGB CHANNEL_COLORS[16] = {
    CRGB::Red,       // Channel 1
    CRGB::Green,     // Channel 2
    CRGB::Blue,      // Channel 3
    CRGB::Yellow,    // Channel 4
    CRGB::Orange,    // Channel 5
    CRGB::Purple,    // Channel 6
    CRGB::Cyan,      // Channel 7
    CRGB::Magenta,   // Channel 8
    CRGB::Pink,      // Channel 9
    CRGB::White,     // Channel 10 (Default Drum channel)
    CRGB::Gold,      // Channel 11
    CRGB::Teal,      // Channel 12
    CRGB::Lavender,  // Channel 13
    CRGB::Lime,      // Channel 14
    CRGB::Aqua,      // Channel 15
    CRGB::DeepPink   // Channel 16
};

#endif // CONSTANTS_H
