/*
  Photo Turntable — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: Stepper
  Wiring: 28BYJ-48 motor plug -> ULN2003 socket
          ULN2003 IN1 -> Pin 8, IN2 -> Pin 9, IN3 -> Pin 10, IN4 -> Pin 11
          ULN2003 + -> 5V, - -> GND   (separate 5V supply for long sessions; connect grounds)
          Optional start button: Pin 2 <-> GND
*/
#include <Stepper.h>

const int STEPS_PER_TURN = 2048;     // 28BYJ-48 in full-step mode
const int SHOTS = 12;                // 12 photos = every 30 degrees
const unsigned long PAUSE_MS = 3000; // time to take each photo
const int BUTTON = 2;

// The Stepper library needs the 28BYJ-48 pins in the order IN1, IN3, IN2, IN4
Stepper motor(STEPS_PER_TURN, 8, 10, 9, 11);

void releaseCoils() {                // switch the coils off so the motor stays cool
  for (int p = 8; p <= 11; p++) digitalWrite(p, LOW);
}

bool waitForStart(unsigned long timeoutMs) {
  unsigned long start = millis();
  while (timeoutMs == 0 || millis() - start < timeoutMs) {
    if (digitalRead(BUTTON) == LOW) { delay(30); while (digitalRead(BUTTON) == LOW) delay(5); return true; }
  }
  return false;
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  motor.setSpeed(8);                 // RPM (this motor tops out around 15)
  Serial.begin(9600);
  Serial.println("Press the button to start (auto-start in 5 s the first time)");
  waitForStart(5000);
}

void loop() {
  int done = 0;
  for (int shot = 1; shot <= SHOTS; shot++) {
    // Calculate exact targets so rounding errors don't add up
    int target = (long)shot * STEPS_PER_TURN / SHOTS;
    motor.step(target - done);
    done = target;
    releaseCoils();
    Serial.print("Shot ");
    Serial.print(shot);
    Serial.print(" of ");
    Serial.print(SHOTS);
    Serial.println(": take your photo!");
    delay(PAUSE_MS);
  }
  Serial.println("Full circle done. Press the button for another round.");
  waitForStart(0);                   // wait as long as needed
}
