/*
  Fortune Oracle — Parts Lab
  Boards: Arduino Uno / Nano
  Library: LiquidCrystal I2C
  Wiring: MPU-6050: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          LCD (I2C): VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5 (shared)
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if blank
const uint8_t MPU = 0x68;

// Fortunes live in flash memory (PROGMEM). Line 1 | line 2, max 16 characters each.
const char F0[] PROGMEM = "Yes,|definitely!";
const char F1[] PROGMEM = "The stars say:|maybe...";
const char F2[] PROGMEM = "No way.|Not today.";
const char F3[] PROGMEM = "Ask again after|a snack.";
const char F4[] PROGMEM = "Your future is|very shiny!";
const char F5[] PROGMEM = "It is fated.|El... wait, yes.";
const char F6[] PROGMEM = "Signs point|to YES";
const char F7[] PROGMEM = "Unclear. Check|your wiring.";
const char F8[] PROGMEM = "Don't count|on it.";
const char F9[] PROGMEM = "The lab spirits|approve!";
const char* const FORTUNES[] PROGMEM = {F0, F1, F2, F3, F4, F5, F6, F7, F8, F9};
const int COUNT = 10;

long lastMagnitude = 0;

long accelMagnitude() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU, (uint8_t)6);
  long total = 0;
  for (int i = 0; i < 3; i++) {
    uint8_t hi = Wire.read();
    uint8_t lo = Wire.read();
    int16_t v = (int16_t)((hi << 8) | lo);
    total += abs(v);
  }
  return total;
}

void showFortune() {
  char buf[34];
  strcpy_P(buf, (PGM_P)pgm_read_ptr(&FORTUNES[random(COUNT)]));
  char* line2 = strchr(buf, '|');
  if (line2) { *line2 = '\0'; line2++; }
  lcd.clear();
  lcd.print("The spirits");
  lcd.setCursor(0, 1);
  lcd.print("speak");
  for (int i = 0; i < 3; i++) { delay(400); lcd.print('.'); }
  delay(400);
  lcd.clear();
  lcd.print(buf);
  lcd.setCursor(0, 1);
  if (line2) lcd.print(line2);
  delay(3000);
  lcd.clear();
  lcd.print("Ask, then shake");
  lcd.setCursor(0, 1);
  lcd.print("the oracle...");
}

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU);   // wake the MPU-6050
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission();
  lcd.init();
  lcd.backlight();
  randomSeed(analogRead(A0));
  lcd.print("Ask, then shake");
  lcd.setCursor(0, 1);
  lcd.print("the oracle...");
  lastMagnitude = accelMagnitude();
}

void loop() {
  long m = accelMagnitude();
  if (abs(m - lastMagnitude) > 12000) {   // a big change = a shake
    delay(300);
    showFortune();
    m = accelMagnitude();
  }
  lastMagnitude = m;
  delay(30);
}
