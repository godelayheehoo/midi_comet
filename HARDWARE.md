# Hardware Wiring Guide - MIDI Clock LED Comet Visualization

This guide describes how to wire up your ESP32, a WS2812B/NeoPixel LED strip, and a MIDI clock input circuit. It is designed for beginners, providing step-by-step breadboard instructions and explaining the voltages involved.

---

## 1. Bill of Materials (BOM)

Here is a list of components you will need to build this project.

### Core Components
1. **ESP32 Development Board**: (e.g., ESP32 DevKitC, NodeMCU-32S, etc.).
2. **WS2812B (NeoPixel) LED Strip**: A 5V addressable RGB LED strip. A strip with 60 LEDs is ideal for beginners.
3. **Solderless Breadboard & Hookup Wires** (Jumpers).

---

### MIDI Input - Choose One Option

You can build the MIDI input circuit yourself (Option A) or use a pre-built breakout board (Option B).

#### Option A: DIY Optocoupler Circuit (Recommended for learning)
*   **1x 6N137 Optocoupler IC** (DIP-8 package). This is a high-speed chip that isolates the MIDI device's electrical signals from your ESP32, preventing hum and damage.
*   **1x DIN-5 Female Socket** (standard 5-pin MIDI socket).
*   **1x 220 $\Omega$ Resistor** (Red-Red-Brown-Gold).
*   **1x 10 k$\Omega$ Resistor** (Brown-Black-Orange-Gold).
*   **1x 1N4148 (or 1N4001) Diode**.
*   **1x 0.1 $\mu$F Ceramic Capacitor** (marked "104", for noise filtering).

#### Option B: Pre-built MIDI Breakout Module (Easiest)
*   **1x MIDI Breakout Board** (e.g., SparkFun MIDI Shield, Hobbytronics MIDI breakout, or generic Amazon/eBay MIDI modules). These modules have the optocoupler and resistors already soldered onto a small board with a MIDI jack.

---

### Power Components (Based on Strip Length)
*   **For short strips ($\le$ 60 LEDs)**: A standard USB micro-B or USB-C cable connected to a 5V phone charger (2A output is perfect) plugging into the ESP32.
*   **For long strips (> 60 LEDs)**: An external **5V DC regulated power supply** (rated for 3A to 10A depending on the length) and a **1000 $\mu$F 10V (or higher) Electrolytic Capacitor** to buffer the voltage.

---

## 2. Voltage and Power Considerations

1.  **ESP32 Voltage**: The ESP32 chip runs on **3.3V**. However, the development board can accept **5V** via the USB port and convert it to 3.3V using an onboard voltage regulator.
2.  **LED Strip Voltage**: WS2812B LEDs require **5V** for power. Their data input line expects a **5V logic level**. The ESP32 outputs **3.3V logic** on its GPIO pins. While 3.3V logic *often* works directly to control the strip, using a short wire between the ESP32 data pin and the first LED is recommended to prevent data corruption.
3.  **Optocoupler Voltage**: The 6N137 chip will be powered by the ESP32's **3.3V** pin. This ensures the output signal sent to the ESP32's RX pin does not exceed 3.3V, keeping the ESP32 safe.
4.  **Current Draw (Amperage)**:
    *   Each WS2812B LED can draw up to **50mA** at full brightness white. 60 LEDs can draw up to 3.0A.
    *   **However**, because this visualizer animates *comets* (where only a few pixels are lit, mostly faded) and limits maximum brightness (`MAX_BRIGHTNESS = 128`), the current draw is dramatically lower (usually under **400mA** average). This allows you to power a 60-LED strip directly from the ESP32's 5V pin when running on USB.

---

## 3. Step-by-Step Wiring Instructions

Before starting, make sure the ESP32 is **completely unplugged** from USB power.

### Part 1: LED Strip Wiring

This connects the WS2812B strip to the ESP32.

1.  Place the ESP32 dev board onto your breadboard so it straddles the center division, leaving columns open on both sides.
2.  Locate the **5V / VIN / V5** pin on your ESP32 (this pin outputs 5V from the USB socket). Connect a wire from this pin to the **Red wire (+5V)** of the LED strip.
3.  Locate a **GND** pin on your ESP32. Connect a wire from this pin to the **Black wire (GND)** of the LED strip.
4.  Locate **GPIO 13** on your ESP32.
    *   Connect a **330 $\Omega$ resistor** from GPIO 13 to an empty row on the breadboard.
    *   Connect a wire from that **same row** to the **Green (or Yellow) wire (DIN / Data In)** of the LED strip.
    *   *(Note: The resistor protects the first LED from signal spikes).*

---

### Part 2: MIDI Input (Option A: DIY Breadboard Circuit)

If you chose the DIY optocoupler path, use these steps to assemble the circuit. The **6N137** chip has 8 pins. Place it on the breadboard across the center divider, with the small notch or dot facing "up" (representing Pin 1 on the top-left).

#### 6N137 Pin Numbering (Counter-clockwise from top-left):
*   **Pin 1**: Top-left  |  **Pin 8**: Top-right (VCC)
*   **Pin 2**: Mid-left  |  **Pin 7**: Mid-right (Enable)
*   **Pin 3**: Mid-left  |  **Pin 6**: Mid-right (Output)
*   **Pin 4**: Bot-left  |  **Pin 5**: Bot-right (GND)

```
        Notch/Dot
      1 [ o   o ] 8 (VCC)
      2 [     ] 7 (Enable)
      3 [     ] 6 (Output/RX)
      4 [_____] 5 (GND)
```

#### Step-by-Step Optocoupler & DIN-5 Connections:

1.  **Power the Chip**: Connect a wire from the ESP32 **3.3V pin** to the row of **Pin 8** of the 6N137.
2.  **Ground the Chip**: Connect a wire from the ESP32 **GND pin** to the row of **Pin 5** of the 6N137.
3.  **Filter Noise**: Insert a **0.1 $\mu$F ceramic capacitor** directly between **Pin 8** and **Pin 5** (plugging into their respective rows).
4.  **Enable the Chip**: Connect a short jumper wire between **Pin 8** (3.3V) and **Pin 7** (Enable) to pull the enable line HIGH.
5.  **Output Signal**: Connect a wire from **Pin 6** (Output) of the 6N137 to the ESP32 **GPIO 16 (RX2)** pin.
6.  **Pull-Up Resistor**: Insert a **10 k$\Omega$ resistor** between the row of **Pin 8** (3.3V) and the row of **Pin 6** (Output).
7.  **Identify DIN-5 Pins**: Look at the back (wiring side) of your female DIN-5 socket. Locate **Pin 4** (middle-left) and **Pin 5** (middle-right). (Standard pin order left-to-right is 1 - 4 - 2 - 5 - 3).
8.  **DIN-5 Pin 4 to Optocoupler Pin 2**:
    *   Connect a **220 $\Omega$ resistor** to the row of **Pin 2** on the 6N137, extending it to an empty row on the breadboard.
    *   Connect a wire from that **same empty row** to **Pin 4** of the DIN-5 socket.
9.  **DIN-5 Pin 5 to Optocoupler Pin 3**:
    *   Connect a wire directly from the row of **Pin 3** on the 6N137 to **Pin 5** of the DIN-5 socket.
10. **Protection Diode**:
    *   Identify the **1N4148 diode** (it has a line printed on one end, which represents the cathode/negative lead).
    *   Plug the lead *with* the line (cathode) into the row of **Pin 2** on the 6N137.
    *   Plug the lead *without* the line (anode) into the row of **Pin 3** on the 6N137.
    *   *(Note: This protects the internal diode of the 6N137 from reverse voltage spikes).*
11. **Leave DIN-5 Pin 2 (Shield/GND) Unconnected**: Do not connect Pin 2 of the MIDI jack to anything. This breaks electrical loops between instruments!

---

### Part 3: MIDI Input (Option B: Pre-built Breakout Module)

If you chose the pre-built breakout path, follow these steps:

1.  Connect the **VCC** (or 3.3V) pin of the breakout board to the **3.3V pin** of the ESP32.
2.  Connect the **GND** pin of the breakout board to a **GND pin** of the ESP32.
3.  Connect the **TX** (or Out) pin of the breakout board to the **GPIO 16 (RX2)** pin of the ESP32.
4.  *Note: There is no need to wire resistors, diodes, or capacitors—they are integrated into the module.*

---

## 4. Visual Schematic Diagram

Here is a ASCII schematic representation of the breadboard connections:

```
            +-------------------+
            |       ESP32       |
            |                   |
            |               3.3V|-------+--------------+
            |                GND|----+  |              |
            |            GPIO 16|--+ |  |              |
            |            GPIO 13|  | |  |              |
            +-------------------+  | |  |              |
                       |           | |  |              |
                330 Ohm R          | |  |              |
                       |           | |  |              |
            +----------v--------+  | |  |              |
            | LED Strip DIN Pin |  | |  |              |
            +-------------------+  | |  |              |
                                   | |  |              |
                               +---|-|--|--------------|-----------+
                               |   | |  |              | 6N137     |
                               |   | |  |     +--------+           |
                               |   | |  |     |        |           |
                               |  [1 2 3 4]   |     [8 7 6 5]      |
                               +---|--|-|-----|------|-|---|-------+
                                   |  | |     |      | |   |
                                   |  | |     +------+ |   GND (ESP32)
                                   |  | |              |
                                   |  | |              |-- 10k Resistor -- 3.3V
                                   |  | |              |
                                   |  | +--------------|-- GPIO 16 (ESP32)
                                   |  |                |
                                   |  +-- Diode (Line) |
                                   +----- Diode (No line)
                                      |
                                  220 Ohm R
                                      |
                                  [DIN Pin 4]      [DIN Pin 5]
                                  (MIDI Jack)      (MIDI Jack)
```

---

## 5. Verification and First Boot

1.  **Inspect Connections**: Verify that 3.3V power, 5V power, and GND lines are not shorted together.
2.  **Power Up**: Plug the USB cable into your ESP32. The ESP32 status LED should light up, and the LED strip should briefly flicker or show a faint light if they receive power.
3.  **Upload the Code**: Compile and upload your code via PlatformIO:
    ```bash
    /Users/james/.platformio/penv/bin/pio run -t upload
    ```
4.  **Connect MIDI**: Plug a MIDI DIN-5 cable from your music gear (DAW interface, drum machine, hardware sequencer) into the DIN female jack.
5.  **Start Playback**: Set your sequencer to send **MIDI Clock** and press Play. The strip will begin spawning comets synchronized with the tempo. Play notes on any MIDI channel, and the traveling comets will drift in color to match the active instrument tracks!
