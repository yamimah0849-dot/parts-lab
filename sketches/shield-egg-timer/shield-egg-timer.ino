/*
  Shield Egg Timer — Parts Lab
  Boards: Arduino Uno / Mega / Uno R4
  Library: LiquidCrystal
  Hardware: LCD Keypad Shield plugged on top (LCD on pins 8, 9, 4, 5, 6, 7; buttons on A0)
  Optional passive buzzer (+) -> 220 ohm -> Pin 3, (-) -> GND
  UP/DOWN = change, LEFT/RIGHT = minutes/seconds, SELECT = start/stop
*/
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int BUZZER = 3;
// Never touch pin 10 (backlight) on this shield: some clones short out if it's driven HIGH.

enum Button { NONE, RIGHT, UP, DOWN, LEFT, SELECT };

Button readButton() {
  int v = analogRead(A0);       // each button gives a different voltage
  if (v < 60)  return RIGHT;
  if (v < 200) return UP;
  if (v < 400) return DOWN;
  if (v < 600) return LEFT;
  if (v < 800) return SELECT;
  return NONE;
}

int minutes = 5, seconds = 0;
bool editMinutes = true;
bool running = false;
unsigned long endTime = 0;

void draw(long remainingSec) {
  char buf[17];
  lcd.setCursor(0, 0);
  if (running) lcd.print("Running...      ");
  else lcd.print(editMinutes ? "Set: [MIN] sec  " : "Set: min [SEC]  ");
  lcd.setCursor(0, 1);
  snprintf(buf, sizeof(buf), "     %02ld:%02ld      ", remainingSec / 60, remainingSec % 60);
  lcd.print(buf);
}

void setup() {
  lcd.begin(16, 2);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  draw(minutes * 60L + seconds);
}

void loop() {
  Button b = readButton();
  if (b != NONE) {
    Serial.print("button value: ");
    Serial.println(analogRead(A0));
    if (b == SELECT) {
      running = !running;
      if (running) endTime = millis() + (minutes * 60UL + seconds) * 1000UL;
      tone(BUZZER, running ? 1400 : 700, 80);
    } else if (!running) {
      if (b == LEFT) editMinutes = true;
      if (b == RIGHT) editMinutes = false;
      if (b == UP)   { if (editMinutes) minutes = min(99, minutes + 1); else seconds = (seconds + 5) % 60; }
      if (b == DOWN) { if (editMinutes) minutes = max(0, minutes - 1); else seconds = (seconds + 55) % 60; }
    }
    draw(minutes * 60L + seconds);
    while (readButton() != NONE) delay(10);   // wait for release
    delay(30);
  }

  if (running) {
    long left = ((long)endTime - (long)millis()) / 1000;
    if (left <= 0) {
      running = false;
      for (int i = 0; i < 6; i++) {
        lcd.setCursor(0, 0);
        lcd.print(i % 2 ? "                " : "  TIME'S UP!    ");
        tone(BUZZER, 1800, 250);
        delay(400);
      }
      draw(minutes * 60L + seconds);
    } else {
      draw(left);
      delay(100);
    }
  }
}
