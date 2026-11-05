/*
  Desk Drum Bot — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: +12V (barrel jack breakout) -> solenoid -> MOSFET Drain; 1N4007 across the solenoid (stripe to +12V)
          MOSFET Source -> 12V GND and board GND, Gate -> 220 ohm -> Pin 9, 10k ohm Gate -> GND
          Optional start/stop button: Pin 2 <-> GND
          Optional tempo pot: middle -> A0, outer legs -> 5V and GND (set USE_TEMPO_POT = true)
*/

const int SOLENOID = 9;
const int BUTTON = 2;
const bool USE_TEMPO_POT = false;
const int HIT_MS = 30;          // short pulse: a clean tap without overheating
int bpm = 100;

// 16 steps per bar: x = hit, . = rest
const char* PATTERNS[] = {
  "x...x...x...x...",   // four on the floor
  "x..x..x...x.x...",   // syncopated
  "x.x.x.xxx.x.x.xx",   // busy
  "x.....x.x.......",   // half-time
};
const int NUM_PATTERNS = 4;
bool playing = true;

void hit() {
  digitalWrite(SOLENOID, HIGH);
  delay(HIT_MS);
  digitalWrite(SOLENOID, LOW);
}

bool checkButton() {
  if (digitalRead(BUTTON) == LOW) {
    playing = !playing;
    Serial.println(playing ? "Play" : "Stop");
    while (digitalRead(BUTTON) == LOW) delay(5);
    delay(40);
    return true;
  }
  return false;
}

void setup() {
  pinMode(SOLENOID, OUTPUT);
  digitalWrite(SOLENOID, LOW);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  for (int p = 0; p < NUM_PATTERNS; p++) {
    for (int bar = 0; bar < 8; bar++) {
      for (int step = 0; step < 16; step++) {
        checkButton();
        while (!playing) { if (checkButton()) break; delay(10); }
        if (USE_TEMPO_POT) bpm = map(analogRead(A0), 0, 1023, 60, 180);
        unsigned long stepMs = 60000UL / bpm / 4;   // 16th notes
        unsigned long start = millis();
        if (PATTERNS[p][step] == 'x') hit();
        while (millis() - start < stepMs) { }
      }
    }
  }
}
