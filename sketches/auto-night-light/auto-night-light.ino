/*
  Auto Night Light — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: LDR leg 1 -> 5V, LDR leg 2 -> A0
          10k ohm resistor between A0 and GND (voltage divider)
          LED (+) -> 220 ohm -> Pin 9, LED (-) -> GND
  With this wiring the reading is HIGH in light and LOW in darkness.
*/

const int LDR_PIN = A0;
const int LED_PIN = 9;          // PWM pin for dimming

const int DARK_LEVEL = 400;     // below this the room counts as "dark" (tune me!)
const int BLACK_LEVEL = 100;    // at or below this the LED is at full brightness

int smoothed = 1023;            // running average to remove flicker

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int raw = analogRead(LDR_PIN);          // 0 (dark) ... 1023 (bright)
  smoothed = (smoothed * 7 + raw) / 8;    // smooth: 7 parts old, 1 part new

  int brightness = 0;
  if (smoothed < DARK_LEVEL) {
    // darker -> brighter LED (10..255)
    int level = constrain(smoothed, BLACK_LEVEL, DARK_LEVEL);
    brightness = map(level, DARK_LEVEL, BLACK_LEVEL, 10, 255);
  }
  analogWrite(LED_PIN, brightness);

  Serial.print("light: ");
  Serial.print(smoothed);
  Serial.print("   LED: ");
  Serial.println(brightness);
  delay(100);
}
