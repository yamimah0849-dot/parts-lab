/*
  Snake on 8x8 — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: LedControl
  Wiring: MAX7219: VCC -> 5V, GND -> GND, DIN -> 12, CLK -> 11, CS -> 10
          Joystick: +5V -> 5V, GND -> GND, VRx -> A0, VRy -> A1, SW -> Pin 2
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/
#include <LedControl.h>

LedControl lc(12, 11, 10, 1);
const int JOY_X = A0, JOY_Y = A1, JOY_SW = 2, BUZZER = 8;

int snakeX[64], snakeY[64];
int length = 3;
int dirX = 1, dirY = 0;
int foodX, foodY;
bool alive = true;
unsigned long lastStep = 0;
int stepMs = 300;

bool onSnake(int x, int y) {
  for (int i = 0; i < length; i++) if (snakeX[i] == x && snakeY[i] == y) return true;
  return false;
}

void placeFood() {
  do { foodX = random(8); foodY = random(8); } while (onSnake(foodX, foodY));
}

void resetGame() {
  length = 3;
  for (int i = 0; i < length; i++) { snakeX[i] = 3 - i; snakeY[i] = 4; }
  dirX = 1; dirY = 0;
  stepMs = 300;
  alive = true;
  placeFood();
}

void readJoystick() {
  int x = analogRead(JOY_X), y = analogRead(JOY_Y);
  // Only allow turning, never reversing straight into yourself
  if (x < 300 && dirX == 0) { dirX = -1; dirY = 0; }
  else if (x > 700 && dirX == 0) { dirX = 1; dirY = 0; }
  else if (y < 300 && dirY == 0) { dirX = 0; dirY = -1; }
  else if (y > 700 && dirY == 0) { dirX = 0; dirY = 1; }
}

void draw() {
  lc.clearDisplay(0);
  for (int i = 0; i < length; i++) lc.setLed(0, snakeY[i], snakeX[i], true);
  lc.setLed(0, foodY, foodX, (millis() / 150) % 2);   // blinking food
}

void setup() {
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  lc.shutdown(0, false);
  lc.setIntensity(0, 3);
  Serial.begin(9600);
  randomSeed(analogRead(A5));
  resetGame();
}

void loop() {
  if (!alive) {
    if (digitalRead(JOY_SW) == LOW) { while (digitalRead(JOY_SW) == LOW) delay(5); resetGame(); }
    return;
  }
  readJoystick();
  if (millis() - lastStep < (unsigned long)stepMs) { draw(); return; }
  lastStep = millis();

  // New head position (walls are deadly)
  int nx = snakeX[0] + dirX;
  int ny = snakeY[0] + dirY;
  if (nx < 0 || nx > 7 || ny < 0 || ny > 7 || onSnake(nx, ny)) {
    alive = false;
    tone(BUZZER, 150, 500);
    Serial.print("Game over! Score: ");
    Serial.println(length - 3);
    for (int k = 0; k < 3; k++) {          // flash the whole screen
      for (int r = 0; r < 8; r++) lc.setRow(0, r, 0xFF);
      delay(150);
      lc.clearDisplay(0);
      delay(150);
    }
    return;
  }

  bool ate = (nx == foodX && ny == foodY);
  if (ate && length < 64) length++;
  for (int i = length - 1; i > 0; i--) { snakeX[i] = snakeX[i - 1]; snakeY[i] = snakeY[i - 1]; }
  snakeX[0] = nx;
  snakeY[0] = ny;
  if (ate) {
    tone(BUZZER, 1400, 40);
    placeFood();
    stepMs = max(110, stepMs - 12);       // speed up
  }
  draw();
}
