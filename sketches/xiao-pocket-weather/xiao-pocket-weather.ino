/*
  XIAO Pocket Weather — Parts Lab
  Board: Seeed XIAO ESP32-C3   (Tools: USB CDC On Boot: Enabled)
  Libraries: Adafruit AHTX0, Adafruit SSD1306, Adafruit GFX Library
  Wiring: AHT20 and OLED 128x32 share I2C: VCC -> 3V3, GND -> GND, SDA -> D4, SCL -> D5
*/
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_AHTX0 aht;
Adafruit_SSD1306 display(128, 32, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(D4, D5);   // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED not found");
  if (!aht.begin()) Serial.println("AHT20 not found");
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  sensors_event_t hum, temp;
  aht.getEvent(&hum, &temp);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(temp.temperature, 1);
  display.setTextSize(1);
  display.print(" C");
  display.setTextSize(2);
  display.setCursor(0, 17);
  display.print((int)hum.relative_humidity);
  display.setTextSize(1);
  display.print(" % humidity");
  display.display();
  Serial.printf("%.1f C  %.0f %%\n", temp.temperature, hum.relative_humidity);
  delay(2000);
}
