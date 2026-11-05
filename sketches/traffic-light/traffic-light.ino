/*
  Mini Traffic Light — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Red LED (+) -> 220 ohm -> Pin 4
          Yellow LED (+) -> 220 ohm -> Pin 3
          Green LED (+) -> 220 ohm -> Pin 2
          All LED (-) -> GND
          Optional walk button: Pin 7 <-> GND (uses the built-in pull-up)
*/

const int RED_PIN = 4;
const int YELLOW_PIN = 3;
const int GREEN_PIN = 2;
const int BUTTON_PIN = 7;   // optional "walk" button

const unsigned long GREEN_MS  = 6000;
const unsigned long YELLOW_MS = 2000;
const unsigned long RED_MS    = 6000;

// Switch on exactly one light
void showLight(int pin) {
  digitalWrite(RED_PIN,    pin == RED_PIN    ? HIGH : LOW);
  digitalWrite(YELLOW_PIN, pin == YELLOW_PIN ? HIGH : LOW);
  digitalWrite(GREEN_PIN,  pin == GREEN_PIN  ? HIGH : LOW);
}

// Wait up to "ms" milliseconds. Returns true early if the walk button is pressed.
// Without a button the pin stays HIGH (pull-up), so this simply waits.
bool waitOrButton(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (digitalRead(BUTTON_PIN) == LOW) return true;
    delay(10);
  }
  return false;
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  showLight(GREEN_PIN);
  Serial.println("GREEN: cars go");
  if (waitOrButton(GREEN_MS)) {
    Serial.println("Walk button pressed! Changing soon...");
    delay(800);
  }

  showLight(YELLOW_PIN);
  Serial.println("YELLOW: slow down");
  delay(YELLOW_MS);

  showLight(RED_PIN);
  Serial.println("RED: pedestrians walk");
  delay(RED_MS);
}
