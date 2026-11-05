/*
  Party Sound Meter — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: Adafruit NeoPixel
  Wiring: Microphone module analog out (A0 / OUT) -> A0, + -> 5V, G -> GND
          WS2812B 8-LED stick: 5V -> 5V, GND -> GND, DIN -> 330 ohm -> Pin 6
*/
#include <Adafruit_NeoPixel.h>

const int MIC = A0;
const int LED_PIN = 6;
const int N = 8;
int SENSITIVITY = 300;      // lower = more sensitive (peak-to-peak for a full bar)

Adafruit_NeoPixel stick(N, LED_PIN, NEO_GRB + NEO_KHZ800);
float peak = 0;

void setup() {
  stick.begin();
  stick.setBrightness(40);
  Serial.begin(9600);
}

void loop() {
  // Measure loudness: the biggest swing in a 30 ms window
  int lo = 1023, hi = 0;
  unsigned long start = millis();
  while (millis() - start < 30) {
    int v = analogRead(MIC);
    if (v < lo) lo = v;
    if (v > hi) hi = v;
  }
  int swing = hi - lo;
  float level = constrain((float)swing / SENSITIVITY * N, 0, N);
  if (level > peak) peak = level; else peak = max(0.0f, peak - 0.15f);   // falling peak dot

  for (int i = 0; i < N; i++) {
    uint32_t c = 0;
    if (i < (int)level) {
      int r = map(i, 0, N - 1, 0, 255);          // green -> yellow -> red
      c = stick.Color(r, 255 - r, 0);
    }
    if (i == (int)peak && peak > 0.5) c = stick.Color(255, 255, 255);   // white peak dot
    stick.setPixelColor(i, c);
  }
  stick.show();
  Serial.println(swing);
}
