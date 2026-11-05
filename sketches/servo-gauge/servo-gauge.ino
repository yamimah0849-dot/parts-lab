/*
  Knob-to-Servo Gauge — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: Servo
  Wiring: Servo: brown -> GND, red -> 5V, orange -> Pin 9
          Potentiometer: outer legs -> 5V and GND, middle leg -> A0
*/
#include <Servo.h>

Servo pointer;
const int SERVO_PIN = 9;
const int KNOB = A0;
float smoothAngle = 90;

void setup() {
  pointer.attach(SERVO_PIN);
  Serial.begin(9600);
}

void loop() {
  int knob = analogRead(KNOB);                   // 0..1023
  float target = map(knob, 0, 1023, 0, 180);     // 0..180 degrees
  smoothAngle = smoothAngle * 0.85 + target * 0.15;   // smooth out jitter
  pointer.write((int)smoothAngle);
  Serial.println((int)smoothAngle);
  delay(15);
}
