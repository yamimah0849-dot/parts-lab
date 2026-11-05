/*
  Touch Glow Light — Parts Lab
  Board: ESP32-C3 SuperMini   (Tools: USB CDC On Boot: Enabled)
  Library: Adafruit NeoPixel
  Wiring: TTP223 touch: VCC -> 3V3, GND -> GND, I/O -> GPIO 3
          WS2812B 8-LED stick: 5V -> 5V, GND -> GND, DIN -> 330 ohm -> GPIO 4
  Tap = next color, hold = dim.
*/
#include <Adafruit_NeoPixel.h>

const int TOUCH_PIN = 3;
const int LED_PIN = 4;
const int NUM_LEDS = 8;
const int MAX_BRIGHTNESS = 90;

Adafruit_NeoPixel stick(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// warm white, pink, mint, purple, amber, off
const uint32_t COLORS[] = {0xFFB36B, 0xFF4FA3, 0x36F1CD, 0x8B6CFF, 0xFFB547, 0x000000};
const int NUM_COLORS = 6;
int colorIndex = 0;
int brightness = MAX_BRIGHTNESS;

bool touching = false;
unsigned long touchStart = 0;

void show() {
  stick.setBrightness(brightness);
  for (int i = 0; i < NUM_LEDS; i++) stick.setPixelColor(i, COLORS[colorIndex]);
  stick.show();
}

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);
  stick.begin();
  show();
}

void loop() {
  bool now = digitalRead(TOUCH_PIN) == HIGH;   // TTP223 goes HIGH while touched

  if (now && !touching) { touching = true; touchStart = millis(); }

  if (now && touching && millis() - touchStart > 500) {   // holding: dim down
    brightness -= 2;
    if (brightness < 5) brightness = MAX_BRIGHTNESS;      // wrap around to bright
    show();
    delay(30);
  }

  if (!now && touching) {                       // released
    if (millis() - touchStart <= 500) {         // it was a tap
      colorIndex = (colorIndex + 1) % NUM_COLORS;
      Serial.printf("Color %d\n", colorIndex);
      show();
    }
    touching = false;
  }
  delay(10);
}
