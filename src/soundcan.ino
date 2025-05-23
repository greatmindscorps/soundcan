#include <TFT_eSPI.h>
#include <driver/i2s.h>

// ==== USER CONFIGURABLE SECTION ====

// I2S Microphone Pins (adjust for your hardware)
#define I2S_WS   15    // L/R Clock (WS)
#define I2S_SD   32    // Data
#define I2S_SCK  14    // Bit Clock (BCLK)

// Reset Button Pin (active LOW)
#define BUTTON_PIN 0

// LCD dimensions (for 240x135 LCD)
#define SCREEN_W 240
#define SCREEN_H 135

// ==== END USER CONFIGURABLE SECTION ====

#define CAN_X 100
#define CAN_Y 20
#define CAN_W 30
#define CAN_H 90
#define SILENCE_TIMEOUT 2000 // ms before can "locks"
#define CALIBRATION_TIME 2000 // ms for each calibration step

TFT_eSPI tft = TFT_eSPI();
float fillLevel = 0.0; // 0.0 = empty, 1.0 = full
unsigned long lastSoundTime = 0;
bool listening = true;
int16_t minValue = 0;
int16_t maxValue = 0;

// I2S setup for mono 16-bit mic
void setupI2SMic() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD
    };
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

int16_t readMicSample() {
    int16_t sample = 0;
    size_t bytesRead;
    i2s_read(I2S_NUM_0, (char *)&sample, sizeof(sample), &bytesRead, portMAX_DELAY);
    return sample;
}

// Calibrate min (ambient) and max (loud) values
void calibrateMicRange() {
    // Ambient noise calibration
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Knock knock...", SCREEN_W/2, 30);

    unsigned long start = millis();
    int32_t minS = 32767, maxS = -32768;
    while (millis() - start < CALIBRATION_TIME) {
        int16_t val = readMicSample();
        if (val < minS) minS = val;
        if (val > maxS) maxS = val;
        delay(1);
    }
    minValue = minS;

    // Loud sound calibration
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Who's there?", SCREEN_W/2, 30);

    start = millis();
    maxS = minValue + 100; // Fallback in case no loud sound
    while (millis() - start < CALIBRATION_TIME) {
        int16_t val = readMicSample();
        if (val > maxS) maxS = val;
        delay(1);
    }
    maxValue = maxS;
    if (maxValue - minValue < 200) maxValue = minValue + 200; // Prevent zero division
}

void setup() {
    tft.init();
    tft.setRotation(1);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    setupI2SMic();
    calibrateMicRange();
    tft.fillScreen(TFT_BLACK);
    drawCan(0.0);
    lastSoundTime = millis();
    listening = true;
    fillLevel = 0.0;
}

void loop() {
    // Button check (active LOW)
    if (!digitalRead(BUTTON_PIN) && !listening) {
        delay(200); // debounce
        resetCan();
    }

    if (listening) {
        int16_t micValue = readMicSample();
        float mapped = (float)(micValue - minValue) / (maxValue - minValue);
        mapped = constrain(mapped, 0.0, 1.0);

        if (mapped > 0.1) { // Ignore tiny noise
            fillLevel += mapped * 0.01; // Adjust speed as desired
            if (fillLevel > 1.0) fillLevel = 1.0;
            lastSoundTime = millis();
            drawCan(fillLevel);
        }
        if ((millis() - lastSoundTime > SILENCE_TIMEOUT && fillLevel > 0) || fillLevel >= 1.0) {
            listening = false;
            showFinalScore();
        }
    }
}

void drawCan(float fill) {
    tft.fillRect(CAN_X, CAN_Y, CAN_W, CAN_H, TFT_DARKGREY);
    tft.drawRect(CAN_X, CAN_Y, CAN_W, CAN_H, TFT_WHITE);
    int fillPixels = (int)(CAN_H * fill);
    tft.fillRect(CAN_X, CAN_Y + CAN_H - fillPixels, CAN_W, fillPixels, TFT_GREEN);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 100);
    tft.printf("Fill: %d%%", (int)(fill * 100));
}

void showFinalScore() {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 120);
    tft.printf("Final: %d%%", (int)(fillLevel * 100));
    tft.setCursor(10, 135);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("Press button to reset!");
}

void resetCan() {
    fillLevel = 0.0;
    listening = true;
    calibrateMicRange();
    tft.fillScreen(TFT_BLACK);
    drawCan(0.0);
    lastSoundTime = millis();
}

