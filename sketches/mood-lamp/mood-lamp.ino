/*
  Mood Lamp Color Wheel — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  RGB LED: COMMON CATHODE (longest leg -> GND)
  Wiring: Red leg -> 220 ohm -> Pin 9
          Green leg -> 220 ohm -> Pin 10
          Blue leg -> 220 ohm -> Pin 11
          Potentiometer outer legs -> 5V and GND, middle leg -> A0
*/

const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int KNOB_PIN = A0;

// Convert a hue (0..359 degrees on the color wheel) to full-brightness RGB
void setHue(int hue) {
  int r, g, b;
  int x = (hue % 60) * 255 / 60;      // rising part within each 60-degree slice
  switch (hue / 60) {
    case 0:  r = 255;     g = x;       b = 0;   break;   // red -> yellow
    case 1:  r = 255 - x; g = 255;     b = 0;   break;   // yellow -> green
    case 2:  r = 0;       g = 255;     b = x;   break;   // green -> cyan
    case 3:  r = 0;       g = 255 - x; b = 255; break;   // cyan -> blue
    case 4:  r = x;       g = 0;       b = 255; break;   // blue -> magenta
    default: r = 255;     g = 0;       b = 255 - x;      // magenta -> red
  }
  analogWrite(RED_PIN, r);     // common cathode: higher value = brighter
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int knob = analogRead(KNOB_PIN);           // 0..1023
  int hue = map(knob, 0, 1023, 0, 359);
  setHue(hue);
  Serial.print("hue: ");
  Serial.println(hue);
  delay(30);
}
