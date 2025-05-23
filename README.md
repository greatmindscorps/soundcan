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

