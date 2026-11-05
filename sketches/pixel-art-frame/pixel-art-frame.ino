/*
  Pixel Art Frame — Parts Lab
  Board: ESP32 DevKit
  Libraries: Adafruit ST7735 and ST7789 Library, Adafruit GFX Library
  Wiring: 1.8" ST7735 TFT: VCC -> 3V3, GND -> GND, SCK -> GPIO 18, SDA -> GPIO 23,
          CS -> GPIO 5, A0/DC -> GPIO 16, RESET -> GPIO 17, LED -> 3V3
  All scenes are original pixel art drawn from code.
*/
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

Adafruit_ST7735 tft(5, 16, 17);   // CS, DC, RST
const int B = 8;                  // size of one "art pixel" in screen pixels (160x128 -> 20x16 blocks)

uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) { return tft.color565(r, g, b); }
void block(int x, int y, uint16_t c) { tft.fillRect(x * B, y * B, B, B, c); }

void nightSky() {
  tft.fillScreen(rgb(10, 12, 40));
  tft.fillCircle(124, 30, 18, rgb(255, 240, 180));           // moon
  tft.fillCircle(116, 26, 4, rgb(230, 210, 150));
  tft.fillCircle(130, 38, 3, rgb(230, 210, 150));
  for (int x = 0; x < 20; x++) block(x, 15, rgb(20, 60, 40)); // grass
  for (int x = 0; x < 20; x++) if (x % 3) block(x, 14, rgb(20, 60, 40));
  unsigned long end = millis() + 6000;
  while (millis() < end) {                                    // twinkling stars
    int x = random(0, 160), y = random(0, 90);
    tft.drawPixel(x, y, random(2) ? rgb(255, 255, 255) : rgb(10, 12, 40));
    delay(15);
  }
}

void sakuraTree() {
  for (int y = 0; y < 16; y++) tft.fillRect(0, y * B, 160, B, rgb(255, 200 - y * 6, 220 - y * 4));   // pink sky
  for (int y = 9; y < 16; y++) { block(9, y, rgb(90, 50, 40)); block(10, y, rgb(90, 50, 40)); }        // trunk
  int blossoms[][3] = {{80, 50, 26}, {58, 60, 18}, {104, 58, 20}, {80, 30, 18}};
  for (auto& b : blossoms) tft.fillCircle(b[0], b[1], b[2], rgb(255, 120, 170));
  unsigned long end = millis() + 6000;
  while (millis() < end) {                                    // falling petals
    int x = random(10, 150), y = random(70, 128);
    tft.fillCircle(x, y, 1, rgb(255, 150, 190));
    delay(60);
  }
}

void sunset() {
  uint16_t bands[] = {rgb(255, 140, 60), rgb(255, 110, 80), rgb(230, 80, 110), rgb(160, 60, 140), rgb(90, 40, 120)};
  for (int i = 0; i < 5; i++) tft.fillRect(0, i * 16, 160, 16, bands[4 - i]);
  tft.fillCircle(80, 80, 26, rgb(255, 220, 90));                       // sun
  tft.fillTriangle(0, 128, 50, 70, 100, 128, rgb(40, 20, 60));         // mountains
  tft.fillTriangle(60, 128, 120, 60, 160, 128, rgb(60, 30, 80));
  tft.fillRect(0, 112, 160, 16, rgb(30, 15, 45));
  delay(6000);
}

void slimeFriend() {
  uint16_t bg = rgb(30, 40, 70);
  for (int bounce = 0; bounce < 12; bounce++) {
    int squash = (bounce % 2) ? 6 : 0;
    tft.fillScreen(bg);
    tft.fillRoundRect(40, 50 + squash, 80, 60 - squash, 26, rgb(54, 241, 205));   // slime body
    tft.fillCircle(66, 74 + squash, 6, rgb(10, 20, 30));                           // eyes
    tft.fillCircle(94, 74 + squash, 6, rgb(10, 20, 30));
    tft.fillCircle(68, 72 + squash, 2, rgb(255, 255, 255));
    tft.fillCircle(96, 72 + squash, 2, rgb(255, 255, 255));
    tft.fillCircle(58, 86 + squash, 4, rgb(255, 120, 170));                         // blush
    tft.fillCircle(102, 86 + squash, 4, rgb(255, 120, 170));
    tft.fillRect(0, 110, 160, 18, rgb(20, 25, 50));
    delay(500);
  }
}

void setup() {
  tft.initR(INITR_BLACKTAB);   // try INITR_GREENTAB or INITR_REDTAB if colors/edges look wrong
  tft.setRotation(1);          // landscape: 160 x 128
  randomSeed(esp_random());
}

void loop() {
  nightSky();
  sakuraTree();
  sunset();
  slimeFriend();
}
