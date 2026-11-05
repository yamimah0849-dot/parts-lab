/*
  GPS Speedometer — Parts Lab
  Board: ESP32 DevKit
  Libraries: TinyGPSPlus, Adafruit SSD1306, Adafruit GFX Library
  Wiring: NEO-6M GPS: VCC -> VIN (5V), GND -> GND, TX -> GPIO 16, RX -> GPIO 17
          OLED 128x64: VCC -> 3V3, GND -> GND, SDA -> GPIO 21, SCL -> GPIO 22
*/
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int UTC_OFFSET_HOURS = 3;   // Saudi Arabia = UTC+3

TinyGPSPlus gps;
HardwareSerial GPSSerial(2);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 16, 17);   // RX pin 16, TX pin 17
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED not found");
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  while (GPSSerial.available()) gps.encode(GPSSerial.read());   // feed every character to the parser

  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 500) return;
  lastDraw = millis();

  display.clearDisplay();
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    display.setTextSize(1);
    display.setCursor(0, 24);
    display.print("No GPS data: check TX/RX wires");
    display.display();
    return;
  }

  // Speed
  display.setTextSize(3);
  display.setCursor(0, 0);
  if (gps.speed.isValid() && gps.location.isValid()) display.print(gps.speed.kmph(), 0);
  else display.print("--");
  display.setTextSize(1);
  display.print(" km/h");

  // Satellites and fix status
  display.setCursor(0, 34);
  display.print("Sats: ");
  display.print(gps.satellites.isValid() ? gps.satellites.value() : 0);
  display.print(gps.location.isValid() ? "  FIX" : "  searching...");

  // Local time
  display.setCursor(0, 48);
  if (gps.time.isValid()) {
    char buf[12];
    int h = (gps.time.hour() + UTC_OFFSET_HOURS) % 24;
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, gps.time.minute(), gps.time.second());
    display.setTextSize(2);
    display.print(buf);
  }
  display.display();

  if (gps.location.isValid()) {
    Serial.printf("lat %.6f  lng %.6f  %.1f km/h\n", gps.location.lat(), gps.location.lng(), gps.speed.kmph());
  }
}
