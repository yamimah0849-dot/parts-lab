/*
  Door Open Chime — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: MC-38 door sensor: one wire -> Pin 2, other wire -> GND (INPUT_PULLUP)
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/

const int DOOR = 2;
const int BUZZER = 8;
const unsigned long REMIND_AFTER_MS = 60000;   // door open longer than this = reminder beeps

bool wasOpen = false;
unsigned long openedAt = 0;
unsigned long lastRemind = 0;
unsigned long visitors = 0;

void chime() {
  tone(BUZZER, 1568, 180); delay(220);   // G6
  tone(BUZZER, 1319, 300); delay(340);   // E6
}

void setup() {
  pinMode(DOOR, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  wasOpen = digitalRead(DOOR) == HIGH;
}

void loop() {
  bool open = digitalRead(DOOR) == HIGH;   // magnet away = switch open = HIGH
  if (open && !wasOpen) {
    openedAt = millis();
    lastRemind = millis();
    visitors++;
    Serial.print("Door opened. Visitors: ");
    Serial.println(visitors);
    chime();
  }
  if (!open && wasOpen) Serial.println("Door closed");
  wasOpen = open;

  if (open && millis() - openedAt > REMIND_AFTER_MS && millis() - lastRemind > 10000) {
    lastRemind = millis();
    Serial.println("Door still open!");
    tone(BUZZER, 880, 120);
  }
  delay(30);   // also works as a simple debounce
}
