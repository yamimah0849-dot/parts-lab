/*
  Pocket Lab Pet — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: Adafruit SSD1306, Adafruit GFX Library
  Wiring: OLED 128x64: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          SELECT button -> Pin 2, DO button -> Pin 3 (other legs to GND)
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
const int BTN_SELECT = 2, BTN_DO = 3, BUZZER = 8;

int hunger = 80, fun = 80, energy = 80;   // 0..100
bool sleeping = false;
int action = 0;                            // 0 Feed, 1 Play, 2 Sleep
const char* const ACTIONS[3] = {"FEED", "PLAY", "SLEEP"};
unsigned long lastTick = 0;
bool lastSel = false, lastDo = false;

int clamp(int v) { return constrain(v, 0, 100); }

void drawBar(int y, const __FlashStringHelper* label, int value) {
  display.setCursor(0, y);
  display.print(label);
  display.drawRect(42, y, 84, 7, SSD1306_WHITE);
  display.fillRect(43, y + 1, value * 82 / 100, 5, SSD1306_WHITE);
}

void drawPet() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // Face depends on mood
  const char* face = "(^_^)";
  int lowest = min(hunger, min(fun, energy));
  if (sleeping) face = "(-_-)";
  else if (lowest == 0) face = "(T_T)";
  else if (lowest < 30) face = "(>_<)";
  else if (lowest < 60) face = "(o_o)";
  display.setTextSize(2);
  display.setCursor(30, 2);
  display.print(face);
  display.setTextSize(1);
  if (sleeping) { display.setCursor(96, 0); display.print(F("zZ")); }
  drawBar(26, F("Food"), hunger);
  drawBar(36, F("Fun"), fun);
  drawBar(46, F("Energy"), energy);
  display.setCursor(0, 56);
  display.print(F("> "));
  display.print(ACTIONS[action]);
  display.print(F("   [Bitty]"));
  display.display();
}

void beep(int f) { tone(BUZZER, f, 60); }

void setup() {
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_DO, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println(F("OLED not found")); while (true) delay(100); }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 24);
  display.print(F("*hatch!*"));
  display.display();
  beep(880); delay(150); beep(1320);
  delay(1200);
}

void loop() {
  bool sel = digitalRead(BTN_SELECT) == LOW;
  bool act = digitalRead(BTN_DO) == LOW;
  if (sel && !lastSel) { action = (action + 1) % 3; beep(1500); }
  if (act && !lastDo) {
    if (action == 0) { hunger = clamp(hunger + 30); sleeping = false; beep(1000); }
    if (action == 1) { if (energy > 10) { fun = clamp(fun + 25); energy = clamp(energy - 10); sleeping = false; beep(1800); } else beep(200); }
    if (action == 2) { sleeping = !sleeping; beep(600); }
  }
  lastSel = sel;
  lastDo = act;

  // Every 6 seconds the stats change a little
  if (millis() - lastTick > 6000) {
    lastTick = millis();
    hunger = clamp(hunger - (sleeping ? 1 : 2));
    fun = clamp(fun - (sleeping ? 0 : 2));
    energy = clamp(energy + (sleeping ? 5 : -1));
    if (sleeping && energy >= 100) { sleeping = false; beep(1200); }   // woke up!
  }
  drawPet();
  delay(40);
}
