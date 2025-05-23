# SoundCan

A fun, general-purpose ESP32 project that visually fills a can on a 240x135 LCD based on how loud and long you scream, laugh, or clap.  
Fully hardware-agnostic: works with any ESP32, I2S MEMS microphone (e.g., INMP441, ICS-43434, SPM1423), and 240x135 LCD (e.g., ST7789).

## Features

- Uses I2S digital MEMS microphone for accurate sound measurement
- Calibrates both ambient (min) and maximum (max) sound at startup
- Animated can fills up as sound is detected, mapped to full mic range
- Reset button support (configurable pin)
- All original graphics and code—no copyrighted content

## Hardware

- **ESP32 dev board** (any model)
- **I2S MEMS microphone** (e.g., INMP441, ICS-43434, SPM1423)
- **240x135 LCD** (e.g., ST7789, use [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) or similar)
- **Pushbutton** (for reset, any GPIO)

## Pin Configuration

Edit these `#define` lines in `soundcan.ino` for your hardware:


// I2S Microphone Pins
#define I2S_WS 15 // L/R Clock (WS)
#define I2S_SD 32 // Data
#define I2S_SCK 14 // Bit Clock (BCLK)

// LCD Pins: configure in TFT_eSPI's User_Setup.h

// Reset Button
#define BUTTON_PIN 0 // GPIO for reset button (active LOW)


## How It Works

1. On startup, the device calibrates ambient noise ("Knock knock...").
2. Then it asks for a loud sound to calibrate the max ("Who's there?").
3. The can fills up as sound is detected, using the full calibrated range.
4. After a period of silence, the can stays filled.
5. Press the reset button to recalibrate and start over.

## Getting Started

1. Wire up your I2S mic and LCD to your ESP32.  
   - Set the correct pin numbers in `soundcan.ino` and TFT_eSPI's User_Setup.h.
2. Install the [TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI) in Arduino IDE.
3. Open `src/soundcan.ino`.
4. Select your ESP32 board in Arduino IDE.
5. Upload and enjoy!

## License

MIT

## Developed and tested using M5StickC Plus 2 (ESP32, SPM1423 I2S mic, 240x135 LCD), but easily adaptable to any ESP32/I2S mic/LCD combo.
