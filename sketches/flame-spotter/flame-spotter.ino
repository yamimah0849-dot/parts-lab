/*
  Flame Spotter (demo) — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  NOT a fire alarm.
  Wiring: Flame sensor: VCC -> 5V, GND -> GND, DO -> Pin 2, AO -> A0
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Optional red LED (+) -> 220 ohm -> Pin 13, (-) -> GND
  Adjust the sensor's screw so its DO light is ON with a flame and OFF without.
*/

const int FLAME_DO = 2;
const int FLAME_AO = A0;
const int BUZZER = 8;
const int LED = 13;
const bool FLAME_WHEN_LOW = true;   // most modules pull DO LOW when they see a flame

void setup() {
  pinMode(FLAME_DO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  bool flame = (digitalRead(FLAME_DO) == LOW) == FLAME_WHEN_LOW;
  int strength = 1023 - analogRead(FLAME_AO);   // most modules: lower AO = stronger IR
  Serial.print(flame ? "FLAME!  " : "clear   ");
  Serial.print("strength: ");
  Serial.println(strength);

  if (flame) {
    digitalWrite(LED, HIGH);
    tone(BUZZER, 2200, 120);
    delay(160);
    digitalWrite(LED, LOW);
    delay(160);
  } else {
    delay(200);
  }
}
