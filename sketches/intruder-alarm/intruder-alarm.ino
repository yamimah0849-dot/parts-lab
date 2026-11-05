/*
  Intruder Alarm — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: PIR HC-SR501: VCC -> 5V, OUT -> Pin 2, GND -> GND
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Optional arm/disarm button: Pin 4 <-> GND
          Optional red LED (+) -> 220 ohm -> Pin 13, (-) -> GND
*/

const int PIR = 2;
const int BUZZER = 8;
const int BUTTON = 4;
const int LED = 13;
const unsigned long EXIT_DELAY_MS = 10000;   // time to leave the room after arming
const unsigned long SIREN_MS = 10000;        // how long the siren plays

enum State { DISARMED, ARMING, ARMED };
State state = ARMING;                        // arms itself after power-up
unsigned long stateSince = 0;

bool buttonPressed() {
  if (digitalRead(BUTTON) == LOW) {
    delay(30);                                 // debounce
    while (digitalRead(BUTTON) == LOW) delay(5);
    return true;
  }
  return false;
}

void siren() {
  Serial.println("!!! MOTION DETECTED !!!");
  unsigned long start = millis();
  while (millis() - start < SIREN_MS) {
    for (int f = 600; f < 1600; f += 20) {     // rising wail
      tone(BUZZER, f);
      digitalWrite(LED, (f / 100) % 2);
      delay(4);
      if (buttonPressed()) { noTone(BUZZER); state = DISARMED; Serial.println("Disarmed"); return; }
    }
    for (int f = 1600; f > 600; f -= 20) {     // falling wail
      tone(BUZZER, f);
      delay(4);
    }
  }
  noTone(BUZZER);
}

void setup() {
  pinMode(PIR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Alarm: arming in 10 s (the PIR also needs ~1 min to settle)");
  stateSince = millis();
}

void loop() {
  if (buttonPressed()) {
    if (state == DISARMED) { state = ARMING; stateSince = millis(); Serial.println("Arming..."); tone(BUZZER, 1000, 100); }
    else { state = DISARMED; Serial.println("Disarmed"); tone(BUZZER, 500, 200); }
  }

  switch (state) {
    case DISARMED:
      digitalWrite(LED, LOW);
      break;
    case ARMING:                                  // blink during the exit delay
      digitalWrite(LED, (millis() / 250) % 2);
      if (millis() - stateSince > EXIT_DELAY_MS) { state = ARMED; Serial.println("ARMED"); tone(BUZZER, 1500, 150); }
      break;
    case ARMED:
      digitalWrite(LED, HIGH);
      if (digitalRead(PIR) == HIGH) siren();
      break;
  }
  delay(20);
}
