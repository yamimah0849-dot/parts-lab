/*
  Light Harp — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: LDR from 5V to A0, 10k ohm from A0 to GND
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
  Keep your hand away during the first 3 seconds (calibration).
*/

const int LDR = A0;
const int BUZZER = 8;
int brightLevel = 0;   // reading with no shadow (measured at startup)

void setup() {
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  long sum = 0;
  for (int i = 0; i < 60; i++) { sum += analogRead(LDR); delay(50); }   // 3 seconds
  brightLevel = sum / 60;
  Serial.print("Room light level: ");
  Serial.println(brightLevel);
  tone(BUZZER, 880, 100);
}

void loop() {
  int v = analogRead(LDR);
  int shadow = brightLevel - v;              // how much darker than the room
  if (shadow > 40) {
    int freq = map(constrain(shadow, 40, brightLevel), 40, brightLevel, 220, 1760);
    tone(BUZZER, freq);
  } else {
    noTone(BUZZER);
  }
  delay(20);
}
