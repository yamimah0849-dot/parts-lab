/*
  Desert Runner (LCD game) — Parts Lab
  Boards: Arduino Uno / Nano
  Library: LiquidCrystal I2C
  Wiring: LCD (I2C): GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
          Jump button: Pin 2 <-> GND
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if the screen stays blank
const int BUTTON = 2, BUZZER = 8;

// Custom 5x8 characters (each row is 5 pixels)
byte runA[8]   = {0b01100, 0b01100, 0b00100, 0b01110, 0b10101, 0b00100, 0b01010, 0b10001};
byte runB[8]   = {0b01100, 0b01100, 0b00100, 0b01110, 0b10101, 0b00100, 0b01010, 0b01010};
byte cactus[8] = {0b00100, 0b00101, 0b10101, 0b10101, 0b10111, 0b11100, 0b00100, 0b00100};

int obstacleCol[3];
int jumpTicks = 0;
unsigned long score = 0, best = 0;
int tickMs = 180;
bool playing = false;
bool lastBtn = false;

void resetGame() {
  for (int i = 0; i < 3; i++) obstacleCol[i] = 16 + i * 7 + random(0, 4);
  jumpTicks = 0;
  score = 0;
  tickMs = 180;
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, runA);
  lcd.createChar(1, runB);
  lcd.createChar(2, cactus);
  randomSeed(analogRead(A0));
  lcd.setCursor(1, 0);
  lcd.print("DESERT RUNNER");
  lcd.setCursor(1, 1);
  lcd.print("press to start");
}

void loop() {
  bool btn = digitalRead(BUTTON) == LOW;
  bool tapped = btn && !lastBtn;
  lastBtn = btn;

  if (!playing) {
    if (tapped) { resetGame(); playing = true; lcd.clear(); }
    delay(20);
    return;
  }

  if (tapped && jumpTicks == 0) { jumpTicks = 4; tone(BUZZER, 900, 40); }

  static unsigned long lastTick = 0;
  if (millis() - lastTick < (unsigned long)tickMs) return;
  lastTick = millis();

  // move the cacti left and recycle them
  for (int i = 0; i < 3; i++) {
    obstacleCol[i]--;
    if (obstacleCol[i] < 0) {
      int far = 0;
      for (int k = 0; k < 3; k++) far = max(far, obstacleCol[k]);
      obstacleCol[i] = max(16, far + random(4, 9));
    }
  }
  bool inAir = jumpTicks > 0;
  if (jumpTicks > 0) jumpTicks--;

  // crash: a cactus in column 1 while the runner is on the ground
  for (int i = 0; i < 3; i++) {
    if (obstacleCol[i] == 1 && !inAir) {
      playing = false;
      if (score > best) best = score;
      tone(BUZZER, 150, 500);
      lcd.clear();
      lcd.print("OUCH! Score ");
      lcd.print(score);
      lcd.setCursor(0, 1);
      lcd.print("Best ");
      lcd.print(best);
      lcd.print("  (press)");
      delay(800);
      return;
    }
  }

  score++;
  if (score % 25 == 0 && tickMs > 70) tickMs -= 10;   // speed up

  lcd.clear();
  for (int i = 0; i < 3; i++) {
    if (obstacleCol[i] >= 0 && obstacleCol[i] < 16) { lcd.setCursor(obstacleCol[i], 1); lcd.write(byte(2)); }
  }
  lcd.setCursor(1, inAir ? 0 : 1);
  lcd.write(byte(score % 2));          // alternate the two running frames
  lcd.setCursor(12, 0);
  lcd.print(score);
}
