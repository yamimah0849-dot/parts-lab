/*
  Bike Tamper Alarm — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Nano Every
  Wiring: SW-420 vibration module: VCC -> 5V, GND -> GND, DO -> Pin 2
          Arm/disarm button: Pin 4 <-> GND
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Optional red LED (+) -> 220 ohm -> Pin 13, (-) -> GND
*/

const int VIBRATION = 2, BUTTON = 4, BUZZER = 8, LED = 13;
const unsigned long EXIT_DELAY_MS = 10000;
const unsigned long SIREN_MS = 15000;
const int SHAKES_NEEDED = 3;          // changes on DO within the window
const unsigned long WINDOW_MS = 2000;

enum State { DISARMED, ARMING, ARMED };
State state = DISARMED;
unsigned long stateSince = 0;
int shakes = 0;
unsigned long windowStart = 0;
int lastVib;

bool buttonPressed() {
  if (digitalRead(BUTTON) == LOW) {
    delay(30);
    while (digitalRead(BUTTON) == LOW) delay(5);
    return true;
  }
  return false;
}

void siren() {
  Serial.println("TAMPER! Siren on");
  unsigned long start = millis();
  while (millis() - start < SIREN_MS) {
    for (int f = 900; f < 2200; f += 40) { tone(BUZZER, f); delay(3); }
    digitalWrite(LED, !digitalRead(LED));
    if (buttonPressed()) { state = DISARMED; break; }
  }
  noTone(BUZZER);
  Serial.println(state == DISARMED ? "Disarmed" : "Still armed");
}

void setup() {
  pinMode(VIBRATION, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  lastVib = digitalRead(VIBRATION);
  Serial.println("Press the button to arm");
}

void loop() {
  if (buttonPressed()) {
    if (state == DISARMED) { state = ARMING; stateSince = millis(); tone(BUZZER, 1200, 100); Serial.println("Arming in 10 s"); }
    else { state = DISARMED; tone(BUZZER, 500, 200); Serial.println("Disarmed"); }
  }

  int vib = digitalRead(VIBRATION);
  bool changed = vib != lastVib;   // counting changes works whichever way your module's output goes
  lastVib = vib;

  switch (state) {
    case DISARMED:
      digitalWrite(LED, LOW);
      break;
    case ARMING:
      digitalWrite(LED, (millis() / 250) % 2);
      if (millis() - stateSince > EXIT_DELAY_MS) { state = ARMED; shakes = 0; tone(BUZZER, 1600, 120); Serial.println("ARMED"); }
      break;
    case ARMED:
      digitalWrite(LED, HIGH);
      if (changed) {
        if (millis() - windowStart > WINDOW_MS) { windowStart = millis(); shakes = 0; }
        shakes++;
        if (shakes >= SHAKES_NEEDED) { siren(); shakes = 0; }
      }
      break;
  }
  delay(5);
}
