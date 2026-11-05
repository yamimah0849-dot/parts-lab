/*
  Electronic Dice — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Display: single-digit 7-segment, COMMON CATHODE (e.g. 5161AS)
  Wiring (each segment through its own 220 ohm resistor):
    a -> Pin 2, b -> Pin 3, c -> Pin 4, d -> Pin 5, e -> Pin 6, f -> Pin 7, g -> Pin 8
    Common pin (display pin 3 or 8) -> GND
    Button: Pin 12 <-> GND
    Optional passive buzzer: (+) -> 220 ohm -> Pin 11, (-) -> GND
*/

const int SEG_PINS[7] = {2, 3, 4, 5, 6, 7, 8};   // a, b, c, d, e, f, g
const int BUTTON = 12;
const int BUZZER = 11;

// Bit 0 = segment a ... bit 6 = segment g
const byte DIGITS[7] = {
  0b1000000,   // index 0: a dash (only g)
  0b0000110,   // 1: b c
  0b1011011,   // 2: a b d e g
  0b1001111,   // 3: a b c d g
  0b1100110,   // 4: b c f g
  0b1101101,   // 5: a c d f g
  0b1111101    // 6: a c d e f g
};

void show(byte pattern) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEG_PINS[i], (pattern >> i) & 1 ? HIGH : LOW);   // HIGH = lit (common cathode)
  }
}

void setup() {
  for (int i = 0; i < 7; i++) pinMode(SEG_PINS[i], OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  randomSeed(analogRead(A0));   // floating analog pin = random seed

  show(0b1111111);              // test: all segments on
  delay(600);
  show(DIGITS[0]);              // then a dash
}

void loop() {
  if (digitalRead(BUTTON) == HIGH) return;   // wait for a press

  // Spin: show random numbers, slowing down like a real die
  int d = 30;
  while (d < 320) {
    show(DIGITS[random(1, 7)]);
    tone(BUZZER, 1500, 8);    // click (silent if no buzzer is connected)
    delay(d);
    d = d * 115 / 100;
  }

  int result = random(1, 7);   // 1..6
  show(DIGITS[result]);
  tone(BUZZER, 880, 120);
  Serial.print("You rolled: ");
  Serial.println(result);

  while (digitalRead(BUTTON) == LOW) delay(10);   // wait for release
  delay(50);
}
