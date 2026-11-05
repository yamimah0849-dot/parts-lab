/*
  Precision Stepper Spinner — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Driver: A4988 (full-step: MS1-MS3 unconnected, RESET joined to SLEEP)
  Wiring: STEP -> Pin 3, DIR -> Pin 4, VDD -> 5V, GND -> GND
          VMOT -> +12V with a 100uF capacitor across VMOT/GND, motor GND -> 12V GND (shared with board GND)
          Potentiometer middle -> A0 (outer legs 5V and GND), optional direction button Pin 2 <-> GND
  Set the A4988 current limit BEFORE connecting the motor!
*/

const int STEP_PIN = 3, DIR_PIN = 4, KNOB = A0, DIR_BTN = 2;
const int STEPS_PER_REV = 200;     // NEMA 17 in full-step mode
const int MAX_RPM = 120;

float currentSps = 0;              // steps per second right now
bool forward = true;
unsigned long lastStepUs = 0;
unsigned long lastRampMs = 0;

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(DIR_BTN, INPUT_PULLUP);
  digitalWrite(DIR_PIN, HIGH);
  Serial.begin(9600);
}

void loop() {
  // Every 10 ms: read the knob and ramp the speed gently (big jumps make steppers stall)
  if (millis() - lastRampMs >= 10) {
    lastRampMs = millis();
    int rpm = map(analogRead(KNOB), 0, 1023, 0, MAX_RPM);
    float targetSps = (rpm < 2) ? 0 : rpm * (float)STEPS_PER_REV / 60.0;
    if (currentSps < targetSps) currentSps = min(targetSps, currentSps + 8);
    if (currentSps > targetSps) currentSps = max(targetSps, currentSps - 8);

    if (digitalRead(DIR_BTN) == LOW && currentSps < 20) {   // only reverse when (almost) stopped
      forward = !forward;
      digitalWrite(DIR_PIN, forward ? HIGH : LOW);
      Serial.println(forward ? "Direction: forward" : "Direction: reverse");
      while (digitalRead(DIR_BTN) == LOW) delay(5);
    }
  }

  // Make a step pulse when it's time
  if (currentSps > 0) {
    unsigned long interval = 1000000UL / (unsigned long)currentSps;
    if (micros() - lastStepUs >= interval) {
      lastStepUs = micros();
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(3);         // the A4988 needs at least 1 us
      digitalWrite(STEP_PIN, LOW);
    }
  }
}
