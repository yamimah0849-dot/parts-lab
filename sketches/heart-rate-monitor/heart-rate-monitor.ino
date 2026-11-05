/*
  Heart Rate Monitor (fun, NOT medical) — Parts Lab
  Board: ESP32 DevKit
  Libraries: SparkFun MAX3010x Pulse and Proximity Sensor Library, Adafruit SSD1306, Adafruit GFX Library
  Wiring: MAX30102 and OLED share I2C: VCC -> 3V3, GND -> GND, SDA -> GPIO 21, SCL -> GPIO 22
*/
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

MAX30105 sensor;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const byte RATE_SIZE = 4;      // average the last 4 beats
byte rates[RATE_SIZE];
byte rateSpot = 0;
int filled = 0;
long lastBeat = 0;
int avgBpm = 0;
unsigned long beatFlash = 0;

void drawHeart(int x, int y, int s) {   // s = size
  display.fillCircle(x - s / 2, y, s / 2 + 1, SSD1306_WHITE);
  display.fillCircle(x + s / 2, y, s / 2 + 1, SSD1306_WHITE);
  display.fillTriangle(x - s - 1, y + 1, x + s + 1, y + 1, x, y + s + 3, SSD1306_WHITE);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED not found");
  display.setTextColor(SSD1306_WHITE);
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found: check wiring");
    while (true) delay(100);
  }
  sensor.setup();                       // default settings
  sensor.setPulseAmplitudeRed(0x0A);    // red LED low: just shows it's on
  sensor.setPulseAmplitudeGreen(0);
}

void loop() {
  long ir = sensor.getIR();
  bool finger = ir > 50000;

  if (finger && checkForBeat(ir)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    float bpm = 60.0 / (delta / 1000.0);
    if (bpm > 30 && bpm < 220) {
      rates[rateSpot++] = (byte)bpm;
      rateSpot %= RATE_SIZE;
      if (filled < RATE_SIZE) filled++;
      int sum = 0;
      for (int i = 0; i < filled; i++) sum += rates[i];
      avgBpm = sum / filled;
      beatFlash = millis();
      Serial.printf("beat! bpm %.0f  avg %d\n", bpm, avgBpm);
    }
  }
  if (!finger) { filled = 0; avgBpm = 0; }

  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 80) {
    lastDraw = millis();
    display.clearDisplay();
    if (!finger) {
      display.setTextSize(1);
      display.setCursor(20, 28);
      display.print("Place finger...");
    } else {
      drawHeart(24, 26, millis() - beatFlash < 150 ? 14 : 9);   // the heart "beats"
      display.setTextSize(3);
      display.setCursor(52, 18);
      if (avgBpm > 0) display.print(avgBpm); else display.print("--");
      display.setTextSize(1);
      display.setCursor(56, 48);
      display.print("BPM (not medical)");
    }
    display.display();
  }
}
