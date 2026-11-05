/*
  Tilt Ball Collector — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: Adafruit SSD1306, Adafruit GFX Library
  Wiring: MPU-6050 (GY-521): VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          OLED 128x64:       VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5 (shared)
  Tilt to roll the ball onto the star. Collect 10 stars to win.
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
const uint8_t MPU_ADDR = 0x68;

// If the ball moves the wrong way, flip these
const bool INVERT_X = false;
const bool INVERT_Y = false;
const bool SWAP_XY = false;

float bx = 64, by = 32, vx = 0, vy = 0;   // ball position and speed
int sx, sy;                               // star position
int score = 0;
int16_t offX = 0, offY = 0;               // calibration offsets

// Read the raw X and Y acceleration registers of the MPU-6050
void readAccel(int16_t &ax, int16_t &ay) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                       // ACCEL_XOUT_H register
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)4);
  uint8_t xh = Wire.read();
  uint8_t xl = Wire.read();
  uint8_t yh = Wire.read();
  uint8_t yl = Wire.read();
  ax = (int16_t)((xh << 8) | xl);
  ay = (int16_t)((yh << 8) | yl);
}

void newStar() {
  sx = random(8, 120);
  sy = random(14, 58);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);       // wake the MPU-6050 (it starts asleep)
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found"));
    while (true) delay(100);
  }
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 28);
  display.print(F("Keep flat... "));
  display.display();

  // Calibrate: the average reading while flat becomes "zero tilt"
  long sumX = 0, sumY = 0;
  for (int i = 0; i < 40; i++) {
    int16_t ax, ay;
    readAccel(ax, ay);
    sumX += ax; sumY += ay;
    delay(10);
  }
  offX = sumX / 40;
  offY = sumY / 40;
  randomSeed(analogRead(A0));
  newStar();
}

void loop() {
  int16_t ax, ay;
  readAccel(ax, ay);
  float tx = (ax - offX) / 16384.0;      // tilt in g (1.0 = vertical)
  float ty = (ay - offY) / 16384.0;
  if (SWAP_XY) { float t = tx; tx = ty; ty = t; }
  if (INVERT_X) tx = -tx;
  if (INVERT_Y) ty = -ty;

  // Physics: tilt accelerates the ball, friction slows it
  vx = (vx + ty * 1.6) * 0.96;
  vy = (vy + tx * 1.6) * 0.96;
  bx += vx;
  by += vy;
  if (bx < 3)   { bx = 3;   vx = -vx * 0.6; }   // bounce off the walls
  if (bx > 124) { bx = 124; vx = -vx * 0.6; }
  if (by < 13)  { by = 13;  vy = -vy * 0.6; }
  if (by > 60)  { by = 60;  vy = -vy * 0.6; }

  // Collected the star?
  if (abs(bx - sx) < 6 && abs(by - sy) < 6) {
    score++;
    newStar();
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("STARS "));
  display.print(score);
  display.print(F("/10"));
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  // star: a little plus with diagonals
  display.drawLine(sx - 3, sy, sx + 3, sy, SSD1306_WHITE);
  display.drawLine(sx, sy - 3, sx, sy + 3, SSD1306_WHITE);
  display.drawPixel(sx - 2, sy - 2, SSD1306_WHITE);
  display.drawPixel(sx + 2, sy + 2, SSD1306_WHITE);
  display.drawPixel(sx - 2, sy + 2, SSD1306_WHITE);
  display.drawPixel(sx + 2, sy - 2, SSD1306_WHITE);
  display.fillCircle((int)bx, (int)by, 3, SSD1306_WHITE);
  display.display();

  if (score >= 10) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(16, 16);
    display.print(F("CLEAR!"));
    display.setTextSize(1);
    display.setCursor(22, 44);
    display.print(F("you're a pro ^_^"));
    display.display();
    delay(4000);
    score = 0;
    bx = 64; by = 32; vx = 0; vy = 0;
    newStar();
  }
  delay(25);   // about 40 frames per second
}
