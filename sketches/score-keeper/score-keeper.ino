/*
  Board Game Score Keeper — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Nano Every
  Library: TM1637
  Wiring: Rotary encoder (KY-040): CLK -> 2, DT -> 3, SW -> 4, + -> 5V, GND -> GND
          TM1637: CLK -> 5, DIO -> 6, VCC -> 5V, GND -> GND
  Turn = change score, click = next player, hold 3 s = reset.
*/
#include <TM1637Display.h>

const int ENC_CLK = 2, ENC_DT = 3, ENC_SW = 4;
TM1637Display display(5, 6);

const int PLAYERS = 4;
int scores[PLAYERS] = {0, 0, 0, 0};
int player = 0;
int lastClk;

void show() {
  // left digit: player number, right three digits: score
  uint8_t data[4];
  data[0] = display.encodeDigit(player + 1);
  int s = constrain(abs(scores[player]), 0, 999);
  data[1] = (s >= 100) ? display.encodeDigit(s / 100) : (scores[player] < 0 ? SEG_G : 0);
  data[2] = (s >= 10) ? display.encodeDigit((s / 10) % 10) : 0;
  data[3] = display.encodeDigit(s % 10);
  display.setSegments(data);
}

void setup() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  display.setBrightness(5);
  lastClk = digitalRead(ENC_CLK);
  Serial.begin(9600);
  show();
}

void loop() {
  // Encoder: when CLK changes, DT tells us the direction
  int clk = digitalRead(ENC_CLK);
  if (clk != lastClk && clk == LOW) {
    if (digitalRead(ENC_DT) != clk) scores[player]++;
    else scores[player]--;
    show();
    Serial.print("Player ");
    Serial.print(player + 1);
    Serial.print(": ");
    Serial.println(scores[player]);
  }
  lastClk = clk;

  // Button: click = next player, hold 3 s = reset everything
  if (digitalRead(ENC_SW) == LOW) {
    unsigned long t = millis();
    while (digitalRead(ENC_SW) == LOW && millis() - t < 3000) delay(5);
    if (millis() - t >= 3000) {
      for (int i = 0; i < PLAYERS; i++) scores[i] = 0;
      player = 0;
      Serial.println("All scores reset");
      while (digitalRead(ENC_SW) == LOW) delay(5);
    } else {
      player = (player + 1) % PLAYERS;
    }
    show();
    delay(30);
  }
}
