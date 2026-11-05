/*
  Pocket Altimeter & Barometer — Parts Lab
  Board: ESP32 DevKit
  Libraries: Adafruit BMP280 Library, Adafruit Unified Sensor, Adafruit SSD1306, Adafruit GFX Library
  Wiring: BMP280: VCC -> 3V3, GND -> GND, SCL -> GPIO 22, SDA -> GPIO 21
          OLED:   VCC -> 3V3, GND -> GND, SCL -> GPIO 22, SDA -> GPIO 21 (shared)
*/
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

float SEA_LEVEL_HPA = 1013.25;   // today's sea-level pressure from a weather app makes altitude accurate

Adafruit_BMP280 bmp;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);   // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED not found");
  display.setTextColor(SSD1306_WHITE);

  // Most purple boards use address 0x76, some 0x77
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("BMP280 not found: check wiring (3.3V!)");
    display.clearDisplay();
    display.setCursor(0, 28);
    display.print("BMP280 not found");
    display.display();
    while (true) delay(100);
  }
  // Smooth, accurate readings
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
}

void loop() {
  float hPa = bmp.readPressure() / 100.0;
  float tempC = bmp.readTemperature();
  float altitude = bmp.readAltitude(SEA_LEVEL_HPA);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ALTITUDE");
  display.setTextSize(3);
  display.setCursor(0, 12);
  display.print(altitude, 0);
  display.setTextSize(1);
  display.print(" m");
  display.setCursor(0, 46);
  display.print(hPa, 1);
  display.print(" hPa");
  display.setCursor(0, 56);
  display.print(tempC, 1);
  display.print(" C");
  display.display();

  Serial.printf("%.1f m   %.2f hPa   %.1f C\n", altitude, hPa, tempC);
  delay(500);
}
