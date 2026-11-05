/*
  Binary Counter Trainer — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: LEDs (each via 220 ohm): bit 1 -> Pin 2, bit 2 -> Pin 3, bit 4 -> Pin 4, bit 8 -> Pin 5; all (-) -> GND
          Button: Pin 7 <-> GND
*/

const int LEDS[4] = {2, 3, 4, 5};   // worth 1, 2, 4, 8
const int BUTTON = 7;
int count = 0;

void showNumber(int n) {
  for (int bit = 0; bit < 4; bit++) {
    digitalWrite(LEDS[bit], (n >> bit) & 1 ? HIGH : LOW);   // is this bit a 1?
  }
  Serial.print(n);
  Serial.print("  =  ");
  for (int bit = 3; bit >= 0; bit--) Serial.print((n >> bit) & 1);
  Serial.println();
}

void setup() {
  for (int i = 0; i < 4; i++) pinMode(LEDS[i], OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  showNumber(0);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    unsigned long t = millis();
    while (digitalRead(BUTTON) == LOW && millis() - t < 1000) delay(5);
    if (millis() - t >= 1000) count = 0;          // held: reset
    else count = (count + 1) % 16;                // tapped: +1
    showNumber(count);
    while (digitalRead(BUTTON) == LOW) delay(5);
    delay(40);
  }
}
