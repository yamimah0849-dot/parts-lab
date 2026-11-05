/*
  Banana Touch Piano — Parts Lab
  Board: ESP32 DevKit (the classic ESP32 has touch-sensing pins)
  Wiring: Touch keys (just wires, foil or fruit): GPIO 4, 15, 13, 14, 27, 33, 32
          Passive buzzer (+) -> 220 ohm -> GPIO 25, (-) -> GND
  Don't touch the keys during the first 2 seconds (calibration).
*/

const int KEYS[7] = {4, 15, 13, 14, 27, 33, 32};
const int NOTES[7] = {262, 294, 330, 349, 392, 440, 494};   // C D E F G A B
const char* NAMES[7] = {"C", "D", "E", "F", "G", "A", "B"};
const int BUZZER = 25;

int baseline[7];   // "not touched" value for each key

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  delay(1000);
  // Measure each key while nobody touches it
  for (int k = 0; k < 7; k++) {
    long sum = 0;
    for (int i = 0; i < 20; i++) { sum += touchRead(KEYS[k]); delay(10); }
    baseline[k] = sum / 20;
  }
  Serial.println("Ready! Touch a key.");
  tone(BUZZER, 523, 120);
}

void loop() {
  int playing = -1;
  for (int k = 0; k < 7; k++) {
    int v = touchRead(KEYS[k]);
    if (v < baseline[k] * 0.75) { playing = k; break; }   // touched = value drops
  }

  static int last = -2;
  if (playing != last) {
    if (playing >= 0) {
      tone(BUZZER, NOTES[playing]);
      Serial.println(NAMES[playing]);
    } else {
      noTone(BUZZER);
    }
    last = playing;
  }
  delay(15);
}
