/*
  Garage Parking Assistant — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Ultrasonic (HC-SR04 or RCWL-1601): VCC -> 5V, TRIG -> Pin 7, ECHO -> Pin 6, GND -> GND
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Optional LEDs (each via 220 ohm): green -> 2, yellow -> 3, red -> 4
*/

const int TRIG = 7;
const int ECHO = 6;
const int BUZZER = 8;
const int GREEN = 2, YELLOW = 3, RED = 4;

const int FAR_CM = 100;    // start beeping below this distance
const int STOP_CM = 30;    // solid tone and red light below this

// Measure distance in cm. Returns -1 if no echo came back.
long readDistanceCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);       // a 10 microsecond pulse starts a measurement
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long us = pulseIn(ECHO, HIGH, 30000);   // wait max 30 ms (about 5 m)
  if (us == 0) return -1;
  return us / 58;                 // sound: about 58 us per cm (there and back)
}

void lights(bool g, bool y, bool r) {
  digitalWrite(GREEN, g ? HIGH : LOW);
  digitalWrite(YELLOW, y ? HIGH : LOW);
  digitalWrite(RED, r ? HIGH : LOW);
}

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  long cm = readDistanceCm();
  Serial.print("Distance: ");
  Serial.print(cm);
  Serial.println(" cm");

  if (cm < 0 || cm > FAR_CM) {          // nothing close (or out of range)
    lights(true, false, false);
    noTone(BUZZER);
    delay(150);
  } else if (cm > STOP_CM) {            // getting closer: beep faster
    lights(false, true, false);
    int pause = map(cm, STOP_CM, FAR_CM, 60, 600);
    tone(BUZZER, 1200, 60);
    delay(60 + pause);
  } else {                              // stop!
    lights(false, false, true);
    tone(BUZZER, 1200);
    delay(100);
  }
}
