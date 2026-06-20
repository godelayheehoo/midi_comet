# MIDI Clock LED Comet Visualization
## Goal

Create a WS2812/NeoPixel LED strip visualization synchronized to MIDI Clock.

The visualization should display multiple comet-like pulses traveling along the strip. New comets are launched at every 16th note. Each comet should take exactly one bar (4/4 time) to travel from the beginning of the strip to the end.

The effect should function as a visual representation of musical time and measure progression.

## Hardware
WS2812 / NeoPixel compatible LED strip
LED count configurable via constant
MIDI Clock input
MIDI Start / Stop support preferred
Configuration Constants

Provide these user-editable constants:

- NUM_LEDS            // total LEDs in strip
- BASE_HUE            // normal comet color
- DOWNBEAT_HUE        // color used on beat 1
- MAX_BRIGHTNESS      // global brightness limit
- COMET_LENGTH        // tail length in LEDs
- FPS                 // render refresh rate

Assume 4/4 time.

## MIDI Clock Handling

MIDI Clock sends:

24 clocks per quarter note
96 clocks per measure (4/4)

Maintain an internal clock counter.

Detect:

- MIDI Start
- MIDI Stop
- MIDI Clock

Reset measure position on MIDI Start.

## Comet Launching

Launch a new comet every 16th note.

Since:

- 24 clocks per quarter note
- 6 clocks per 16th note

Launch a comet every 6 MIDI clock ticks.

This results in:

16 comets launched per bar

## Comet Motion

Each comet should:

Start at position 0
Reach the final LED exactly one bar later

**Travel time:**

96 MIDI clock ticks

Motion should be smooth.

Do NOT advance only on 16th notes.

**Instead:**

Track fractional position
Update continuously using clock timing

**Position equation:**

progress = age_in_clocks / 96.0

position = progress * (NUM_LEDS - 1)

A comet expires when:

- age_in_clocks >= 96
## Comet Appearance

Each comet consists of:

- Bright head
- Smooth fading tail

Brightness should decay smoothly behind the head.

**Example:**

Ooooo....

Use linear or quadratic falloff.

Tail length determined by COMET_LENGTH.

The head should be the brightest point.

## Colors

Normal 16th-note launches:

BASE_HUE

The first 16th note of each measure (beat 1) should use:

DOWNBEAT_HUE

Example:

Normal comets = deep blue
Downbeat comet = white

No rainbow cycling.

No note-based colors.

No channel-based colors.

Keep the color scheme simple and musically readable.

## Rendering

Maintain a list of active comets.

For each frame:

Clear LED buffer.
Render all active comets.
Blend overlapping comets additively.
Clamp brightness to valid LED range.
Output frame.

Multiple comets should be visible simultaneously.

The strip should appear to contain a stream of moving comets.

## Visual Behavior

Desired visual effect:

- A new comet appears every 16th note.
- Comets move continuously.
- Every comet requires exactly one measure to traverse the strip.
- The strip continuously displays measure progression.
- The first comet of each measure is visually distinct through color.
- Motion should feel smooth and elegant rather than flashy.
## Stretch Goals

Optional enhancements:

Fade-In/Fade-Out

Allow comet heads to ease in and out slightly.

Tempo Change Handling

Automatically adapt to changing MIDI Clock tempo.

Reverse Direction Mode

Optional configuration to travel from strip end to strip start.

Gamma Correction

Apply LED gamma correction for smoother perceived brightness.

Double-Length Travel

Allow a configuration option where a comet takes 2 bars instead of 1 bar to traverse the strip.

Success Criteria

When connected to a MIDI clock source:

- LEDs remain synchronized indefinitely.
- One comet launches every 16th note.
- Each comet reaches the strip end exactly one bar after launch.
- Beat 1 of every measure is visually obvious.
- The animation remains smooth at all tempos.
- Overlapping comets blend cleanly without flicker.

Possible color changes: 
- Change based on last instrument (midi channel) used
- Activity-weighted color

  - Keep a short decay value for each MIDI channel.

  - Whenever a note arrives:

  - channel_activity[channel] = 1.0

  - Then decay over time:

  -     channel_activity *= 0.98

  - Each comet's color becomes a blend of the channel colors weighted by activity.

  - So if:

  - drums are busy
  - bass is moderately active
  - pad is sustained

  - you might get a pale blue-white color.

  - Then when the lead comes in, the comets gradually drift toward magenta.