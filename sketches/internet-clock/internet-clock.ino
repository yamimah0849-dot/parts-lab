/*
  Internet Time Clock — Parts Lab
  Board: ESP32 DevKit
  Library: TM1637 (Avishay Orpaz)
  Wiring: TM1637: VCC -> 3V3, GND -> GND, CLK -> GPIO 18, DIO -> GPIO 19
*/
#include <WiFi.h>
#include <time.h>
#include <TM1637Display.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME
// POSIX time zone. Saudi Arabia is UTC+3, written "<+03>-3" (the sign is reversed in this format).
const char* TIMEZONE = "<+03>-3";

TM1637Display display(18, 19);   // CLK, DIO
const uint8_t DASHES[] = { SEG_G, SEG_G, SEG_G, SEG_G };

void setup() {
  Serial.begin(115200);
  display.setBrightness(5);
  display.setSegments(DASHES);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println(" connected!");

  // Ask internet time servers for the exact time (NTP)
  configTzTime(TIMEZONE, "pool.ntp.org", "time.google.com");
}

void loop() {
  struct tm now;
  if (!getLocalTime(&now, 200)) {           // not synced yet
    display.setSegments(DASHES);
    return;
  }
  int value = now.tm_hour * 100 + now.tm_min;
  bool colon = now.tm_sec % 2 == 0;
  display.showNumberDecEx(value, colon ? 0b01000000 : 0, true);

  static int lastSec = -1;
  if (now.tm_sec != lastSec) {
    lastSec = now.tm_sec;
    Serial.printf("%02d:%02d:%02d\n", now.tm_hour, now.tm_min, now.tm_sec);
  }
  delay(100);
}
