/*
  Knob Dimmer — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Potentiometer outer legs -> GND and 5V, middle leg -> A0
          LED (+) -> 220 ohm -> Pin 9, LED (-) -> GND
*/

const int KNOB_PIN = A0;
const int LED_PIN = 9;     // must be a PWM pin (marked ~ on the Uno)

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int knob = analogRead(KNOB_PIN);              // 0 .. 1023
  int brightness = map(knob, 0, 1023, 0, 255);  // convert to 0 .. 255
  analogWrite(LED_PIN, brightness);

  Serial.print("knob: ");
  Serial.print(knob);
  Serial.print("  -> brightness: ");
  Serial.println(brightness);
  delay(50);
}
