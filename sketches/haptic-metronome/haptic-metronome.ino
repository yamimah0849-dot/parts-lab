/*
  Silent Haptic Metronome — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: NPN transistor (2N2222/PN2222A): Base -> 1k ohm -> Pin 9, Emitter -> GND, Collector -> motor (-)
          Vibration motor (+) -> 5V, 1N4007 across the motor (stripe toward 5V)
          Potentiometer: outer legs -> 5V and GND, middle -> A0
          Optional LED (+) -> 220 ohm -> Pin 13, (-) -> GND
*/

const int MOTOR = 9;        // PWM pin to the transistor base
const int KNOB = A0;
const int LED = 13;
const int BUZZ_PWM = 150;   // 150/255 of 5V = about 3V average for this 3V motor
const int BEATS_PER_BAR = 4;

int beat = 0;
unsigned long nextBeat = 0;
int lastBpm = 0;

void setup() {
  pinMode(MOTOR, OUTPUT);
  pinMode(LED, OUTPUT);
  analogWrite(MOTOR, 0);
  Serial.begin(9600);
}

void loop() {
  int bpm = map(analogRead(KNOB), 0, 1023, 40, 200);
  if (abs(bpm - lastBpm) > 1) {
    Serial.print("Tempo: ");
    Serial.print(bpm);
    Serial.println(" BPM");
    lastBpm = bpm;
  }

  if (millis() >= nextBeat) {
    nextBeat = millis() + 60000UL / bpm;
    int buzzMs = (beat == 0) ? 120 : 45;   // beat 1 of the bar is longer
    analogWrite(MOTOR, BUZZ_PWM);
    digitalWrite(LED, HIGH);
    delay(buzzMs);
    analogWrite(MOTOR, 0);
    digitalWrite(LED, LOW);
    beat = (beat + 1) % BEATS_PER_BAR;
  }
}
