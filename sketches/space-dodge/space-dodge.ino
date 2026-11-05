/*
  Space Dodge — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: Adafruit SSD1306, Adafruit GFX Library
  Wiring: OLED: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          Joystick: +5V -> 5V, GND -> GND, VRx -> A0, VRy -> A1, SW -> Pin 2
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
const int JOY_X = A0, JOY_Y = A1, JOY_SW = 2, BUZZER = 8;
const int ROCKS = 6;

float shipX = 60, shipY = 52;
float rockX[ROCKS], rockY[ROCKS], rockV[ROCKS];
int rockR[ROCKS];
unsigned long startTime = 0;
unsigned long best = 0;
bool playing = false;

void newRock(int i) {
  rockX[i] = random(4, 124);
  rockY[i] = -random(5, 60);
  rockR[i] = random(2, 5);
  float level = (millis() - startTime) / 15000.0;   // gets harder every 15 s
  rockV[i] = 0.8 + random(0, 10) / 10.0 + level * 0.5;
}

void startGame() {
  shipX = 60; shipY = 52;
  startTime = millis();
  for (int i = 0; i < ROCKS; i++) newRock(i);
  playing = true;
}

void setup() {
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println(F("OLED not found")); while (true) delay(100); }
  display.setTextColor(SSD1306_WHITE);
  randomSeed(analogRead(A3));
}

void loop() {
  if (!playing) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(4, 8);
    display.print(best ? F("GAME OVER") : F("SPACE DODGE"));
    display.setTextSize(1);
    display.setCursor(4, 36);
    display.print(F("best: "));
    display.print(best / 1000.0, 1);
    display.print(F(" s"));
    display.setCursor(4, 52);
    display.print(F("click stick to fly"));
    display.display();
    if (digitalRead(JOY_SW) == LOW) { while (digitalRead(JOY_SW) == LOW) delay(5); startGame(); }
    return;
  }

  // Move the ship with the joystick
  shipX += (analogRead(JOY_X) - 512) / 150.0;
  shipY += (analogRead(JOY_Y) - 512) / 150.0;
  shipX = constrain(shipX, 3, 124);
  shipY = constrain(shipY, 12, 60);

  display.clearDisplay();
  for (int i = 0; i < ROCKS; i++) {
    rockY[i] += rockV[i];
    if (rockY[i] > 70) newRock(i);
    display.drawCircle((int)rockX[i], (int)rockY[i], rockR[i], SSD1306_WHITE);
    // collision: distance between centers smaller than the sizes added up
    float dx = rockX[i] - shipX, dy = rockY[i] - shipY;
    if (dx * dx + dy * dy < (rockR[i] + 3) * (rockR[i] + 3)) {
      unsigned long t = millis() - startTime;
      if (t > best) best = t;
      for (int r = 2; r < 30; r += 3) { display.drawCircle((int)shipX, (int)shipY, r, SSD1306_WHITE); display.display(); tone(BUZZER, 600 - r * 15, 30); delay(30); }
      Serial.print(F("Survived "));
      Serial.print(t / 1000.0, 1);
      Serial.println(F(" s"));
      delay(600);
      playing = false;
      return;
    }
  }
  // ship: a small triangle
  display.fillTriangle((int)shipX, (int)shipY - 4, (int)shipX - 3, (int)shipY + 3, (int)shipX + 3, (int)shipY + 3, SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print((millis() - startTime) / 1000);
  display.print(F(" s"));
  display.display();
  delay(20);
}
