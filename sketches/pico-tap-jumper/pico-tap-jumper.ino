/*
  Tap Jumper (OLED game) — Parts Lab
  Boards: Raspberry Pi Pico / Pico W
  Libraries: Adafruit SSD1306, Adafruit GFX Library
  Wiring: OLED: VCC -> 3V3(OUT), GND -> GND, SDA -> GP4, SCL -> GP5
          Button: GP15 <-> GND
          Optional passive buzzer (+) -> 220 ohm -> GP14, (-) -> GND
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
const int BTN = 15;
const int BUZZER = 14;
const int GROUND_Y = 56;     // y of the ground line
const int PLAYER_X = 16;
const int PLAYER_SIZE = 8;

float py, vy;                // player top y and vertical speed
bool onGround = true;
float obsX[3];
int obsH[3];
float speed;
unsigned long score = 0, best = 0;
bool playing = false;
bool lastBtn = false;

void resetGame() {
  py = GROUND_Y - PLAYER_SIZE;
  vy = 0;
  speed = 2.0;
  score = 0;
  for (int i = 0; i < 3; i++) { obsX[i] = 140 + i * 70 + random(0, 40); obsH[i] = random(6, 13); }
}

float farthest() {
  float m = 0;
  for (int i = 0; i < 3; i++) if (obsX[i] > m) m = obsX[i];
  return m;
}

void drawWorld() {
  display.clearDisplay();
  display.drawLine(0, GROUND_Y, 127, GROUND_Y, SSD1306_WHITE);
  // player: a little block with an eye
  display.fillRect(PLAYER_X, (int)py, PLAYER_SIZE, PLAYER_SIZE, SSD1306_WHITE);
  display.drawPixel(PLAYER_X + 5, (int)py + 2, SSD1306_BLACK);
  // spikes
  for (int i = 0; i < 3; i++) {
    int x = (int)obsX[i];
    display.fillTriangle(x, GROUND_Y, x + 6, GROUND_Y, x + 3, GROUND_Y - obsH[i], SSD1306_WHITE);
  }
  display.setTextSize(1);
  display.setCursor(80, 0);
  display.print(score / 10);
}

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED not found");
  display.setTextColor(SSD1306_WHITE);
  randomSeed(analogRead(26));
  resetGame();
}

void loop() {
  bool btn = digitalRead(BTN) == LOW;
  bool tapped = btn && !lastBtn;   // react only to new presses
  lastBtn = btn;

  if (!playing) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(4, 6);
    display.print(score ? "GAME OVER" : "TAP JUMP!");
    display.setTextSize(1);
    display.setCursor(4, 34);
    display.print("score ");
    display.print(score / 10);
    display.print("  best ");
    display.print(best / 10);
    display.setCursor(4, 50);
    display.print("tap to start");
    display.display();
    if (tapped) { resetGame(); playing = true; }
    delay(30);
    return;
  }

  // jump
  if (tapped && onGround) { vy = -4.6; tone(BUZZER, 880, 40); }
  vy += 0.35;                       // gravity
  py += vy;
  if (py >= GROUND_Y - PLAYER_SIZE) { py = GROUND_Y - PLAYER_SIZE; vy = 0; onGround = true; }
  else onGround = false;

  // move spikes, recycle the ones that left the screen
  for (int i = 0; i < 3; i++) {
    obsX[i] -= speed;
    if (obsX[i] < -8) { obsX[i] = max(farthest(), 128.0f) + random(50, 110); obsH[i] = random(6, 13); }
    // collision: player box vs spike box
    bool overlapX = obsX[i] < PLAYER_X + PLAYER_SIZE - 1 && obsX[i] + 6 > PLAYER_X + 1;
    bool overlapY = py + PLAYER_SIZE > GROUND_Y - obsH[i] + 2;
    if (overlapX && overlapY) {
      playing = false;
      if (score > best) best = score;
      tone(BUZZER, 160, 400);
      drawWorld();
      display.display();
      delay(800);
      return;
    }
  }

  score++;
  speed = min(5.0f, 2.0f + score / 600.0f);   // slowly speeds up
  drawWorld();
  display.display();
  delay(30);
}
