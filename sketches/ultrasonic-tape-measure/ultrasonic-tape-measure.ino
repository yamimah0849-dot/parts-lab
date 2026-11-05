/*
  Ultrasonic Tape Measure — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: LiquidCrystal
  Wiring (16x2 LCD, no I2C, 4-bit mode):
    VSS -> GND, VDD -> 5V, V0 -> potentiometer middle (pot ends to 5V and GND)
    RS -> Pin 12, RW -> GND, E -> Pin 11, D4 -> 5, D5 -> 4, D6 -> 3, D7 -> 2
    A (backlight +) -> 220 ohm -> 5V, K (backlight -) -> GND
  Ultrasonic: VCC -> 5V, TRIG -> Pin 9, ECHO -> Pin 10, GND -> GND
  Optional HOLD button: Pin 7 <-> GND
*/
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // RS, E, D4, D5, D6, D7

const int TRIG = 9;
const int ECHO = 10;
const int HOLD_BTN = 7;

float readCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long us = pulseIn(ECHO, HIGH, 30000);
  if (us == 0) return -1;
  return us / 58.0;
}

void setup() {
  lcd.begin(16, 2);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(HOLD_BTN, INPUT_PULLUP);
  lcd.print("Tape Measure");
  lcd.setCursor(0, 1);
  lcd.print("Parts Lab");
  delay(1200);
}

void loop() {
  bool hold = digitalRead(HOLD_BTN) == LOW;
  if (hold) {                     // keep the last value on screen
    lcd.setCursor(12, 0);
    lcd.print("HOLD");
    delay(100);
    return;
  }

  // Average 3 readings for a steadier number
  float sum = 0;
  int good = 0;
  for (int i = 0; i < 3; i++) {
    float cm = readCm();
    if (cm > 0) { sum += cm; good++; }
    delay(30);
  }

  lcd.clear();
  if (good == 0) {
    lcd.print("Out of range");
  } else {
    float cm = sum / good;
    lcd.print(cm, 1);
    lcd.print(" cm");
    lcd.setCursor(0, 1);
    lcd.print(cm / 2.54, 1);
    lcd.print(" in");
  }
  delay(160);
}
