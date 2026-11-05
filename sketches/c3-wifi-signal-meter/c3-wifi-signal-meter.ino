/*
  Wi-Fi Signal Meter — Parts Lab
  Board: ESP32-C3 SuperMini   (Tools: USB CDC On Boot: Enabled)
  Libraries: Adafruit SSD1306, Adafruit GFX Library
  Wiring: OLED 128x32: VCC -> 3V3, GND -> GND, SDA -> GPIO 5, SCL -> GPIO 6
*/
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* TARGET_SSID = "";   // your network's name, or "" to show the strongest one

Adafruit_SSD1306 display(128, 32, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(5, 6);   // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED not found");
  display.setTextColor(SSD1306_WHITE);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void loop() {
  int n = WiFi.scanNetworks();
  int best = -1;
  for (int i = 0; i < n; i++) {
    bool match = strlen(TARGET_SSID) == 0 || WiFi.SSID(i) == TARGET_SSID;
    if (match && (best < 0 || WiFi.RSSI(i) > WiFi.RSSI(best))) best = i;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (best < 0) {
    display.print("Searching...");
  } else {
    int rssi = WiFi.RSSI(best);
    String name = WiFi.SSID(best);
    if (name.length() > 21) name = name.substring(0, 21);
    display.print(name);
    // bar: -90 dBm (empty) ... -40 dBm (full)
    int width = map(constrain(rssi, -90, -40), -90, -40, 0, 92);
    display.drawRect(0, 14, 94, 12, SSD1306_WHITE);
    display.fillRect(1, 15, width, 10, SSD1306_WHITE);
    display.setCursor(98, 16);
    display.print(rssi);
    Serial.printf("%s  %d dBm\n", name.c_str(), rssi);
  }
  display.display();
  WiFi.scanDelete();
  delay(300);
}
