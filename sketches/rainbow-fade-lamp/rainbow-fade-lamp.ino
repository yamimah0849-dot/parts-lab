/*
  Rainbow Fade Lamp — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  RGB LED: COMMON ANODE (longest leg -> 5V). A color lights when its pin goes LOW.
  Wiring: Red leg -> 220 ohm -> Pin 9
          Green leg -> 220 ohm -> Pin 10
          Blue leg -> 220 ohm -> Pin 11
          Button: Pin 2 <-> GND
*/

const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int BUTTON = 2;

const int SPEEDS[3] = {60, 25, 8};   // milliseconds per hue step: slow, medium, fast
int speedIndex = 0;
int hue = 0;

// Common anode: 255 - value, so 255 (full) becomes 0 (pin LOW = LED on)
void writeColor(int r, int g, int b) {
  analogWrite(RED_PIN, 255 - r);
  analogWrite(GREEN_PIN, 255 - g);
  analogWrite(BLUE_PIN, 255 - b);
}

void setHue(int h) {
  int x = (h % 60) * 255 / 60;
  switch (h / 60) {
    case 0:  writeColor(255, x, 0); break;
    case 1:  writeColor(255 - x, 255, 0); break;
    case 2:  writeColor(0, 255, x); break;
    case 3:  writeColor(0, 255 - x, 255); break;
    case 4:  writeColor(x, 0, 255); break;
    default: writeColor(255, 0, 255 - x);
  }
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    speedIndex = (speedIndex + 1) % 3;
    Serial.print("Speed mode: ");
    Serial.println(speedIndex + 1);
    while (digitalRead(BUTTON) == LOW) delay(10);   // wait for release
    delay(40);                                      // debounce
  }
  setHue(hue);
  hue = (hue + 1) % 360;
  delay(SPEEDS[speedIndex]);
}
