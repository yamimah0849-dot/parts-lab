/*
  Anime Robot Eye — Parts Lab
  Board: ESP32 DevKit
  Libraries: Adafruit GC9A01A, Adafruit GFX Library
  Wiring: GC9A01 round TFT: VCC -> 3V3, GND -> GND, SCL -> GPIO 18, SDA -> GPIO 23,
          CS -> GPIO 5, DC -> GPIO 16, RST -> GPIO 17, BL -> 3V3
*/
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 17

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16* canvas;              // off-screen image: no flicker

uint16_t IRIS_DARK, IRIS_MID, IRIS_LIGHT, PUPIL, SCLERA, LID;

float eyeX = 0, eyeY = 0;         // current iris offset
float targetX = 0, targetY = 0;
int lid = 0;                      // 0 = open .. 120 = closed
int blinkPhase = 0;               // 0 = none, 1 = closing, 2 = opening
unsigned long nextLook = 0, nextBlink = 2000, surpriseUntil = 0, nextSurprise = 15000;

void drawEye() {
  canvas->fillScreen(LID);
  canvas->fillCircle(120, 120, 108, SCLERA);                          // white of the eye
  int ix = 120 + (int)eyeX, iy = 120 + (int)eyeY;
  bool surprised = millis() < surpriseUntil;
  int irisR = surprised ? 46 : 54;
  canvas->fillCircle(ix, iy, irisR, IRIS_DARK);                       // anime iris: dark top...
  canvas->fillCircle(ix, iy + irisR / 4, irisR * 3 / 4, IRIS_MID);    // ...lighter toward the bottom
  canvas->fillCircle(ix, iy + irisR / 2, irisR / 3, IRIS_LIGHT);
  canvas->fillCircle(ix, iy, surprised ? 10 : 22, PUPIL);             // pupil
  canvas->fillCircle(ix + 20, iy - 22, surprised ? 15 : 12, SCLERA);  // big sparkle
  canvas->fillCircle(ix - 20, iy + 20, 5, SCLERA);                    // small sparkle
  if (lid > 0) {                                                      // eyelids for blinking
    canvas->fillRect(0, 0, 240, lid, LID);
    canvas->fillRect(0, 240 - lid, 240, lid, LID);
  }
  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 240, 240);
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  canvas = new GFXcanvas16(240, 240);
  IRIS_DARK = tft.color565(60, 20, 140);     // CHANGE ME: iris colors
  IRIS_MID = tft.color565(120, 80, 255);
  IRIS_LIGHT = tft.color565(54, 241, 205);
  PUPIL = tft.color565(15, 5, 40);
  SCLERA = tft.color565(255, 255, 255);
  LID = tft.color565(10, 10, 20);
  randomSeed(esp_random());
}

void loop() {
  unsigned long now = millis();
  if (now > nextLook) {                       // pick a new place to look
    float a = random(0, 628) / 100.0;
    float r = random(0, 40);
    targetX = cos(a) * r;
    targetY = sin(a) * r;
    nextLook = now + random(800, 2200);
  }
  eyeX += (targetX - eyeX) * 0.25;            // ease toward the target
  eyeY += (targetY - eyeY) * 0.25;

  if (blinkPhase == 0 && now > nextBlink) blinkPhase = 1;
  if (blinkPhase == 1) { lid += 30; if (lid >= 120) { lid = 120; blinkPhase = 2; } }
  else if (blinkPhase == 2) { lid -= 20; if (lid <= 0) { lid = 0; blinkPhase = 0; nextBlink = now + random(2500, 6000); } }

  if (now > nextSurprise) { surpriseUntil = now + 900; nextSurprise = now + random(12000, 25000); Serial.println("!!"); }

  drawEye();
}
