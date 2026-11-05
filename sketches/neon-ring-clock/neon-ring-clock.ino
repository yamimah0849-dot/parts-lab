/*
  Neon Ring Clock — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: Adafruit NeoPixel, RTClib
  Wiring: WS2812B 16-LED ring: 5V -> 5V, GND -> GND, DIN -> 330 ohm -> Pin 6
          DS3231: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
*/
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>

const int RING_PIN = 6;
const int N = 16;
const bool REVERSED = false;   // set true if the hands go counter-clockwise
const int OFFSET = 0;          // which LED is at 12 o'clock

Adafruit_NeoPixel ring(N, RING_PIN, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;

uint8_t r[N], g[N], b[N];

int pos(int p) {
  p = (p + OFFSET) % N;
  return REVERSED ? (N - p) % N : p;
}

void add(int i, uint8_t rr, uint8_t gg, uint8_t bb) {   // mix colors where hands overlap
  r[i] = max(r[i], rr); g[i] = max(g[i], gg); b[i] = max(b[i], bb);
}

void setup() {
  ring.begin();
  ring.setBrightness(40);      // calm night glow (and low current)
  Serial.begin(9600);
  if (!rtc.begin()) { Serial.println("DS3231 not found"); while (true) delay(100); }
  if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();
  for (int i = 0; i < N; i++) { r[i] = 0; g[i] = 0; b[i] = 0; }

  int hourPos = ((now.hour() % 12) * 60 + now.minute()) * N / 720;
  int minPos = now.minute() * N / 60;
  int secPos = now.second() * N / 60;

  add(pos(minPos), 0, 200, 220);     // cyan minutes
  add(pos(hourPos), 255, 40, 140);   // pink hour
  if (now.second() % 2 == 0) add(pos(secPos), 255, 200, 0);   // twinkling yellow seconds

  for (int i = 0; i < N; i++) ring.setPixelColor(i, ring.Color(r[i], g[i], b[i]));
  ring.show();
  delay(250);
}
