/*
  Ultrasonic Theremin — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: Ultrasonic: VCC -> 5V, GND -> GND, TRIG -> Pin 7, ECHO -> Pin 6
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/

const int TRIG = 7, ECHO = 6, BUZZER = 8;
const bool SMOOTH = false;   // true = sliding sci-fi sound, false = musical notes

// A pentatonic scale over two octaves (C D E G A): always sounds nice
const int SCALE[] = {262, 294, 330, 392, 440, 523, 587, 659, 784, 880, 1047};
const int STEPS = sizeof(SCALE) / sizeof(SCALE[0]);

long readCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long us = pulseIn(ECHO, HIGH, 6000);   // ~1 m max: keeps it snappy
  return us == 0 ? 999 : us / 58;
}

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  long cm = readCm();
  if (cm >= 5 && cm <= 50) {
    if (SMOOTH) {
      tone(BUZZER, map(cm, 5, 50, 1500, 200));
    } else {
      int step = map(cm, 5, 50, STEPS - 1, 0);   // closer = higher
      tone(BUZZER, SCALE[step]);
    }
  } else {
    noTone(BUZZER);
  }
  delay(30);
}
