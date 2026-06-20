#include <Arduino.h>
#include <FastLED.h>
#include <MIDI.h>
#include "constants.h"

// Calculate maximum comets based on travel duration
#define MAX_COMETS (32 * COMET_TRAVEL_BARS)

// Comet data structure
struct Comet {
    bool active = false;
    float age_in_clocks = 0.0f; // Integer clock ticks elapsed since launch
    bool is_downbeat = false;
    CRGB color = CRGB::Black;
};

// Global variables
CRGB leds[NUM_LEDS];
Comet comets[MAX_COMETS];

// MIDI control variables
volatile bool is_playing = true; // Default to true so we visualize clocks even if no Start message is sent
volatile bool pending_start = false;
volatile int clock_counter = -1; // -1 means no clock has been processed yet
volatile bool first_clock_detected = false;

// Time interpolation variables
volatile uint32_t last_clock_micros = 0;
volatile float avg_tick_duration_micros = 0.0f;

// MIDI channel activity levels (decayed in render loop)
volatile float channel_activity[16] = {0.0f};

// Instantiate MIDI on Hardware Serial 2
HardwareSerial midiSerial(2);
MIDI_CREATE_INSTANCE(HardwareSerial, midiSerial, MIDI);

// Forward declarations
void launchComet(bool is_downbeat);
CRGB get_activity_weighted_color();
void renderFrame();

// ==========================================
// MIDI Callbacks
// ==========================================

void handleClock() {
    uint32_t now = micros();
    
    // Measure duration between clocks to compute average tick duration
    if (last_clock_micros != 0) {
        uint32_t interval = now - last_clock_micros;
        // Simple sanity check: ignore intervals > 2 seconds (corresponds to < 1.8 BPM)
        if (interval < 2000000) {
            if (avg_tick_duration_micros == 0.0f) {
                avg_tick_duration_micros = interval;
            } else {
                // Exponential moving average to smooth out clock jitter
                avg_tick_duration_micros = (avg_tick_duration_micros * 7.0f + interval) / 8.0f;
            }
        }
    }
    last_clock_micros = now;

#ifdef DEBUG_MODE
    bool is_first_clock = false;
    if (!first_clock_detected) {
        Serial.println("[DEBUG] First MIDI Clock tick detected!");
        first_clock_detected = true;
        is_first_clock = true;
    }
#endif

    // Handle starting alignment
    if (pending_start) {
        clock_counter = 0;
        pending_start = false;
    } else {
        clock_counter++;
        if (clock_counter >= 96) {
            clock_counter = 0;
        }
    }

    // Launch a new comet every 16th note (every 6 MIDI clock ticks)
    if (clock_counter % 6 == 0) {
        launchComet(clock_counter == 0);
#ifdef DEBUG_MODE
        if (!is_first_clock) {
            float bpm = 0.0f;
            if (avg_tick_duration_micros > 0.0f) {
                bpm = 60000000.0f / (avg_tick_duration_micros * 24.0f);
            }
            Serial.printf("[DEBUG] 16th Note. Clock counter: %d. Est BPM: %.1f\n", clock_counter, bpm);
        }
#endif
    }

    // Advance age of all active comets by exactly 1 tick.
    // We round to the nearest integer to resolve any fractional numeric drift.
    for (int i = 0; i < MAX_COMETS; i++) {
        if (comets[i].active) {
            comets[i].age_in_clocks = round(comets[i].age_in_clocks) + 1.0f;
            if (comets[i].age_in_clocks >= 96.0f * COMET_TRAVEL_BARS) {
                comets[i].active = false;
            }
        }
    }
}

void handleStart() {
    is_playing = true;
    pending_start = true;
    first_clock_detected = false;
    
    // Clear the strip and active comets for a fresh start
    for (int i = 0; i < MAX_COMETS; i++) {
        comets[i].active = false;
    }
}

void handleContinue() {
    is_playing = true;
}

void handleStop() {
    is_playing = false;
}

void handleNoteOn(byte channel, byte note, byte velocity) {
    if (velocity > 0 && channel >= 1 && channel <= 16) {
        channel_activity[channel - 1] = 1.0f;
    }
}

// ==========================================
// Comet Logic
// ==========================================

void launchComet(bool is_downbeat) {
    int slot = -1;
    
    // Find first inactive slot
    for (int i = 0; i < MAX_COMETS; i++) {
        if (!comets[i].active) {
            slot = i;
            break;
        }
    }

    // If all slots are full, steal the slot of the oldest active comet
    if (slot == -1) {
        float max_age = -1.0f;
        for (int i = 0; i < MAX_COMETS; i++) {
            if (comets[i].active && comets[i].age_in_clocks > max_age) {
                max_age = comets[i].age_in_clocks;
                slot = i;
            }
        }
    }

    if (slot != -1) {
        comets[slot].active = true;
        comets[slot].age_in_clocks = 0.0f;
        comets[slot].is_downbeat = is_downbeat;

        // Assign colors based on downbeat or channel activity
        if (is_downbeat) {
            if (DOWNBEAT_IS_WHITE) {
                comets[slot].color = CRGB::White;
            } else {
                comets[slot].color = CHSV(DOWNBEAT_HUE, DOWNBEAT_SATURATION, 255);
            }
        } else {
            if (ENABLE_CHANNEL_ACTIVITY_COLORING) {
                comets[slot].color = get_activity_weighted_color();
            } else {
                comets[slot].color = CHSV(BASE_HUE, 255, 255);
            }
        }
    }
}

// Blends the colors of active MIDI channels based on their current activity levels
CRGB get_activity_weighted_color() {
    float total_activity = 0.0f;
    for (int i = 0; i < 16; i++) {
        total_activity += channel_activity[i];
    }

    // If no recent MIDI note activity, fall back to the BASE_HUE color
    if (total_activity < 0.01f) {
        return CHSV(BASE_HUE, 255, 255);
    }

    float r = 0.0f, g = 0.0f, b = 0.0f;
    for (int i = 0; i < 16; i++) {
        float weight = channel_activity[i] / total_activity;
        r += CHANNEL_COLORS[i].r * weight;
        g += CHANNEL_COLORS[i].g * weight;
        b += CHANNEL_COLORS[i].b * weight;
    }

    return CRGB((uint8_t)r, (uint8_t)g, (uint8_t)b);
}

// ==========================================
// Rendering
// ==========================================

void renderFrame() {
    // 1. Clear LED buffer (FastLED clear is highly optimized)
    memset(leds, 0, NUM_LEDS * sizeof(CRGB));

    // Calculate fractional clock tick elapsed since the last physical clock message
    float fractional_tick = 0.0f;
    if (is_playing && avg_tick_duration_micros > 0.0f && last_clock_micros != 0) {
        uint32_t now = micros();
        uint32_t elapsed = now - last_clock_micros;
        fractional_tick = (float)elapsed / avg_tick_duration_micros;
        
        // Clamp to 1.0f to avoid overshooting before the next clock tick arrives
        if (fractional_tick > 1.0f) {
            fractional_tick = 1.0f;
        }
    }

    // 2. Render each active comet onto the LED buffer
    for (int c = 0; c < MAX_COMETS; c++) {
        if (!comets[c].active) continue;

        // Total age including sub-tick interpolation
        float current_age = comets[c].age_in_clocks + fractional_tick;
        float progress = current_age / (96.0f * COMET_TRAVEL_BARS);

        // Safety bound: if a comet is fully aged, it will be cleaned up on the next tick
        if (progress > 1.0f) {
            continue;
        }

        // Apply boundary ease-in/ease-out scaling if configured
        float boundary_scale = 1.0f;
        if (FADE_EASE_IN_OUT) {
            if (progress < FADE_LIMIT_PCT) {
                boundary_scale = progress / FADE_LIMIT_PCT;
            } else if (progress > (1.0f - FADE_LIMIT_PCT)) {
                boundary_scale = (1.0f - progress) / FADE_LIMIT_PCT;
            }
        }

        // Calculate theoretical head position (before reverse mapping)
        float head_pos = progress * (NUM_LEDS - 1);

        for (int i = 0; i < NUM_LEDS; i++) {
            // Distance of current LED from the head (positive is in the tail)
            float d = head_pos - i;

            // Render within the tail decay range plus one pixel ahead for anti-aliased leading edge
            if (d >= -1.0f && d <= COMET_LENGTH) {
                float intensity = 0.0f;

                if (d < 0.0f) {
                    // Head anti-aliasing (smoothly fades into the next pixel)
                    intensity = 1.0f + d; // d is negative, so intensity goes 0 -> 1
                } else {
                    // Tail decay: using a quadratic falloff for smoother visual decay
                    float norm_dist = d / COMET_LENGTH;
                    intensity = (1.0f - norm_dist) * (1.0f - norm_dist);
                }

                // Apply boundary ease-in/ease-out scale
                intensity *= boundary_scale;

                if (intensity > 0.001f) {
                    // Determine actual physical LED index based on direction
                    int target_led = REVERSE_DIRECTION ? (NUM_LEDS - 1 - i) : i;

                    CRGB comet_pixel = comets[c].color;
                    comet_pixel.nscale8_video((uint8_t)(intensity * 255.0f));

                    // Additive blending for overlapping comets
                    leds[target_led] += comet_pixel;
                }
            }
        }
    }

    // 3. Apply LED gamma correction if enabled
    if (ENABLE_GAMMA_CORRECTION) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i].r = dim8_video(leds[i].r);
            leds[i].g = dim8_video(leds[i].g);
            leds[i].b = dim8_video(leds[i].b);
        }
    }

    // Push the color data to the LED strip
    FastLED.show();
}

// ==========================================
// Setup and Loop
// ==========================================

void setup() {
    // Optional Serial port for debug output
    Serial.begin(115200);
    Serial.println("MIDI Clock LED Comet Visualizer Initiated.");

    // Initialize FastLED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(MAX_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();

    // Start UART2 at MIDI baud rate (31250)
    midiSerial.begin(31250, SERIAL_8N1, MIDI_RX_PIN, MIDI_TX_PIN);

    // Initialize MIDI Library
    MIDI.begin(MIDI_CHANNEL_OMNI);

    // Set callback handlers
    MIDI.setHandleClock(handleClock);
    MIDI.setHandleStart(handleStart);
    MIDI.setHandleContinue(handleContinue);
    MIDI.setHandleStop(handleStop);
    MIDI.setHandleNoteOn(handleNoteOn);
}

void loop() {
    // Read incoming MIDI messages as fast as possible to minimize jitter
    MIDI.read();

    // Enforce target FPS rendering rate
    uint32_t current_time = millis();
    static uint32_t last_frame_time = 0;
    const uint32_t frame_interval_ms = 1000 / FPS;

    if (current_time - last_frame_time >= frame_interval_ms) {
        last_frame_time = current_time;

        // Decay MIDI channel activity over time
        if (ENABLE_CHANNEL_ACTIVITY_COLORING) {
            for (int i = 0; i < 16; i++) {
                channel_activity[i] *= CHANNEL_ACTIVITY_DECAY;
            }
        }

        // Render and display the current frame
        renderFrame();
    }
}