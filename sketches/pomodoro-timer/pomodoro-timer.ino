/*
  Pomodoro Study Timer — Parts Lab
  Boards: Arduino Uno / Nano
  Library: LiquidCrystal I2C
  Wiring: LCD (I2C): GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
          Button: Pin 2 <-> GND (tap = start/pause, hold 2 s = reset)
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if blank
const int BUTTON = 2, BUZZER = 8;
const int FOCUS_MIN = 25;
const int BREAK_MIN = 5;

bool focusPhase = true;
bool running = false;
unsigned long remainingMs = FOCUS_MIN * 60000UL;
unsigned long lastUpdate = 0;
int rounds = 0;

// Bar pieces: 1 to 5 filled columns
byte bar1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
byte bar2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
byte bar3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
byte bar4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
byte bar5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

unsigned long phaseMs() { return (focusPhase ? FOCUS_MIN : BREAK_MIN) * 60000UL; }

void chime() {
  int notes[] = {784, 988, 1175, 1568};
  for (int n : notes) { tone(BUZZER, n, 150); delay(180); }
}

void drawScreen() {
  lcd.setCursor(0, 0);
  lcd.print(focusPhase ? "FOCUS " : "BREAK ");
  unsigned long s = (remainingMs + 999) / 1000;
  char buf[8];
  snprintf(buf, sizeof(buf), "%02lu:%02lu", s / 60, s % 60);
  lcd.print(buf);
  lcd.print(running ? "  " : " P");   // P = paused
  lcd.setCursor(14, 0);
  lcd.print(rounds % 100);
  // Progress bar: 16 characters x 5 columns = 80 steps
  unsigned long done = phaseMs() - remainingMs;
  int steps = done * 80 / phaseMs();
  lcd.setCursor(0, 1);
  for (int c = 0; c < 16; c++) {
    int fill = constrain(steps - c * 5, 0, 5);
    if (fill == 0) lcd.print(' ');
    else lcd.write(byte(fill - 1));
  }
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, bar1);
  lcd.createChar(1, bar2);
  lcd.createChar(2, bar3);
  lcd.createChar(3, bar4);
  lcd.createChar(4, bar5);
  drawScreen();
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    unsigned long t = millis();
    while (digitalRead(BUTTON) == LOW && millis() - t < 2000) delay(5);
    if (millis() - t >= 2000) {                // long press: reset
      running = false; focusPhase = true; remainingMs = phaseMs(); rounds = 0;
      tone(BUZZER, 400, 200);
      while (digitalRead(BUTTON) == LOW) delay(5);
    } else {                                   // tap: start / pause
      running = !running;
      tone(BUZZER, running ? 1200 : 800, 60);
    }
    lastUpdate = millis();
    drawScreen();
  }

  if (running && millis() - lastUpdate >= 250) {
    unsigned long elapsed = millis() - lastUpdate;
    lastUpdate = millis();
    remainingMs = (elapsed >= remainingMs) ? 0 : remainingMs - elapsed;
    if (remainingMs == 0) {
      if (focusPhase) rounds++;
      focusPhase = !focusPhase;
      remainingMs = phaseMs();
      chime();
      lastUpdate = millis();
    }
    drawScreen();
  }
}
