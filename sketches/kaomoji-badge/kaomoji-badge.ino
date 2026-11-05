/*
  Kaomoji Face Badge — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: LedControl
  Wiring: MAX7219 matrix: VCC -> 5V, GND -> GND, DIN -> 12, CLK -> 11, CS -> 10
          Button: Pin 2 <-> GND   (TTP223 touch: SIG -> Pin 2 and set USE_TOUCH = true)
*/
#include <LedControl.h>

LedControl lc(12, 11, 10, 1);   // DIN, CLK, CS, number of matrices
const int BUTTON = 2;
const bool USE_TOUCH = false;
const bool FLIP = false;        // set true if the faces appear upside down

// Each face = 8 rows, each row = 8 pixels (1 = on, leftmost pixel = highest bit)
const byte FACES[6][8] = {
  {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C},   // happy   ^_^
  {0x3C, 0x42, 0xAD, 0x81, 0xA5, 0x99, 0x42, 0x3C},   // wink    ^_~
  {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18},   // love    <3
  {0x3C, 0x42, 0xA5, 0x99, 0xA5, 0xBD, 0x42, 0x3C},   // angry   >_<
  {0x3C, 0x42, 0x81, 0xE7, 0x81, 0x99, 0x42, 0x3C},   // sleepy  -_-
  {0x3C, 0x42, 0xA5, 0xA5, 0x81, 0x99, 0x5A, 0x3C}    // shocked O_O
};
const char* NAMES[6] = {"happy", "wink", "love", "angry", "sleepy", "shocked"};
int face = 0;

byte reverseBits(byte b) {
  byte r = 0;
  for (int i = 0; i < 8; i++) if (b & (1 << i)) r |= 1 << (7 - i);
  return r;
}

void drawRows(const byte rows[8]) {
  for (int r = 0; r < 8; r++) {
    if (FLIP) lc.setRow(0, 7 - r, reverseBits(rows[r]));
    else lc.setRow(0, r, rows[r]);
  }
}

void blink() {
  if (face == 2) return;            // the heart doesn't blink
  byte rows[8];
  for (int r = 0; r < 8; r++) rows[r] = FACES[face][r];
  rows[2] = 0x81;                   // eyes closed...
  rows[3] = 0xE7;
  drawRows(rows);
  delay(140);
  drawRows(FACES[face]);
}

bool pressed() { return USE_TOUCH ? digitalRead(BUTTON) == HIGH : digitalRead(BUTTON) == LOW; }

void setup() {
  pinMode(BUTTON, USE_TOUCH ? INPUT : INPUT_PULLUP);
  lc.shutdown(0, false);            // wake up the MAX7219
  lc.setIntensity(0, 4);            // brightness 0..15
  lc.clearDisplay(0);
  drawRows(FACES[face]);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  static unsigned long nextBlink = 3000;
  if (pressed()) {
    face = (face + 1) % 6;
    drawRows(FACES[face]);
    Serial.println(NAMES[face]);
    while (pressed()) delay(5);
    delay(40);
  }
  if (millis() > nextBlink) {
    blink();
    nextBlink = millis() + random(2500, 6000);
  }
}
