/*
  Whack-a-LED — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: LEDs (each via 220 ohm) -> Pins 9, 10, 11; LED (-) -> GND
          Buttons -> Pins 2, 3, 4 (other leg to GND)
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/

const int LEDS[3] = {9, 10, 11};
const int BTNS[3] = {2, 3, 4};
const int BUZZER = 8;
const unsigned long ROUND_MS = 30000;

int score = 0;

int pressedButton() {
  for (int i = 0; i < 3; i++) if (digitalRead(BTNS[i]) == LOW) return i;
  return -1;
}

void allLeds(bool on) { for (int i = 0; i < 3; i++) digitalWrite(LEDS[i], on ? HIGH : LOW); }

void setup() {
  for (int i = 0; i < 3; i++) { pinMode(LEDS[i], OUTPUT); pinMode(BTNS[i], INPUT_PULLUP); }
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  Serial.println("Press any button to start a 30 s round!");
  while (pressedButton() < 0) { allLeds((millis() / 300) % 2); }
  while (pressedButton() >= 0) delay(5);
  allLeds(false);
  score = 0;
  delay(800);

  unsigned long roundStart = millis();
  while (millis() - roundStart < ROUND_MS) {
    int mole = random(3);
    unsigned long showMs = max(350, 1000 - score * 30);   // faster as you score
    digitalWrite(LEDS[mole], HIGH);
    unsigned long t = millis();
    int hit = -1;
    while (millis() - t < showMs && hit < 0) hit = pressedButton();
    digitalWrite(LEDS[mole], LOW);

    if (hit == mole) { score++; tone(BUZZER, 1800, 60); }
    else if (hit >= 0) { score = max(0, score - 1); tone(BUZZER, 200, 120); }
    while (pressedButton() >= 0) delay(5);   // wait for release
    delay(random(150, 500));
  }

  Serial.print("Time! Your score: ");
  Serial.println(score);
  for (int k = 0; k < 6; k++) { allLeds(k % 2 == 0); tone(BUZZER, 600 + k * 150, 120); delay(180); }
  allLeds(false);
  delay(1500);
}
