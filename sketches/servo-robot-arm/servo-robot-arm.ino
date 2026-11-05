/*
  4-Servo Robot Arm — Parts Lab
  Boards: Arduino Uno / Nano
  Library: Adafruit PWM Servo Driver Library
  Wiring: PCA9685: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          PCA9685 screw terminal V+ / GND <- 4xAA battery pack (+ / -)   (servo power)
          Servos on channels 0 (base), 1 (shoulder), 2 (elbow), 3 (claw)
          Joystick 1: VRx -> A0 (base), VRy -> A1 (shoulder)
          Joystick 2: VRx -> A2 (elbow), VRy -> A3 (claw)    (+5V -> 5V, GND -> GND)
*/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();   // default address 0x40

const int MIN_PULSE = 120;   // ~0.6 ms at 50 Hz (out of 4096 ticks)
const int MAX_PULSE = 490;   // ~2.4 ms
const int STICKS[4] = {A0, A1, A2, A3};
// Safe angle limits for each joint: reduce them if a servo buzzes at the end
const int MIN_ANGLE[4] = {0, 20, 20, 30};
const int MAX_ANGLE[4] = {180, 160, 160, 120};
const char* const NAMES[4] = {"base", "shoulder", "elbow", "claw"};

float angle[4] = {90, 90, 90, 75};

void writeServo(int ch, float a) {
  int pulse = map((int)a, 0, 180, MIN_PULSE, MAX_PULSE);
  pwm.setPWM(ch, 0, pulse);
}

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);          // servos expect 50 pulses per second
  delay(10);
  for (int i = 0; i < 4; i++) writeServo(i, angle[i]);   // start in a middle pose
}

void loop() {
  for (int i = 0; i < 4; i++) {
    float tilt = (analogRead(STICKS[i]) - 512) / 512.0;   // -1 .. +1
    if (abs(tilt) > 0.15) {                               // dead zone around center
      angle[i] = constrain(angle[i] + tilt * 2.0, MIN_ANGLE[i], MAX_ANGLE[i]);
      writeServo(i, angle[i]);
    }
  }
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    for (int i = 0; i < 4; i++) { Serial.print(NAMES[i]); Serial.print('='); Serial.print((int)angle[i]); Serial.print(' '); }
    Serial.println();
  }
  delay(20);
}
