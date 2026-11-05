/*
  Color Memory Challenge — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: LEDs (each via 220 ohm): red -> 8, yellow -> 9, green -> 10, blue -> 11, all (-) -> GND
          Buttons (other leg to GND): red -> 2, yellow -> 3, green -> 4, blue -> 5
          Passive buzzer (+) -> 220 ohm -> Pin 12, (-) -> GND
*/

const int LEDS[4] = {8, 9, 10, 11};
const int BTNS[4] = {2, 3, 4, 5};
const int TONES[4] = {262, 330, 392, 523};
const int BUZZER = 12;
const int MAX_STEPS = 50;

int sequence[MAX_STEPS];
int length = 0;

void flash(int i, int ms) {
  digitalWrite(LEDS[i], HIGH);
  tone(BUZZER, TONES[i], ms);
  delay(ms);
  digitalWrite(LEDS[i], LOW);
}

// Wait for a button press (or -1 after a timeout)
int waitForButton(unsigned long timeoutMs) {
  unsigned long start = millis();
  while (millis() - start < timeoutMs) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(BTNS[i]) == LOW) {
        flash(i, 200);
        while (digitalRead(BTNS[i]) == LOW) delay(5);   // wait for release
        delay(30);
        return i;
      }
    }
  }
  return -1;
}

void gameOver() {
  Serial.print("Game over! Score: ");
  Serial.println(length - 1);
  for (int f = 400; f > 100; f -= 30) { tone(BUZZER, f, 40); delay(45); }
  for (int k = 0; k < 3; k++) {
    for (int i = 0; i < 4; i++) digitalWrite(LEDS[i], HIGH);
    delay(200);
    for (int i = 0; i < 4; i++) digitalWrite(LEDS[i], LOW);
    delay(200);
  }
  length = 0;
}

void waitForStart() {
  Serial.println("Press any button to start!");
  int i = 0;
  while (true) {
    digitalWrite(LEDS[i], HIGH);
    delay(150);
    digitalWrite(LEDS[i], LOW);
    i = (i + 1) % 4;
    for (int b = 0; b < 4; b++) if (digitalRead(BTNS[b]) == LOW) { while (digitalRead(BTNS[b]) == LOW) delay(5); delay(500); return; }
  }
}

void setup() {
  for (int i = 0; i < 4; i++) { pinMode(LEDS[i], OUTPUT); pinMode(BTNS[i], INPUT_PULLUP); }
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  if (length == 0) waitForStart();

  sequence[length] = random(4);       // add one more step
  length++;
  Serial.print("Round ");
  Serial.println(length);

  int speed = max(180, 500 - length * 15);   // flashes get quicker
  for (int s = 0; s < length; s++) { flash(sequence[s], speed); delay(speed / 2); }

  for (int s = 0; s < length; s++) {
    int pressed = waitForButton(4000);
    if (pressed != sequence[s]) { gameOver(); return; }
  }
  delay(600);
  if (length >= MAX_STEPS) { Serial.println("You beat the whole game! Legendary!"); length = 0; }
}
