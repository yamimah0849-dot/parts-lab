/*
  Reaction Speed Duel — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: GO LED (+) -> 220 ohm -> Pin 9
          Player 1 LED (+) -> 220 ohm -> Pin 10
          Player 2 LED (+) -> 220 ohm -> Pin 11
          All LED (-) -> GND
          Player 1 button: Pin 2 <-> GND, Player 2 button: Pin 3 <-> GND
          Optional passive buzzer: (+) -> 220 ohm -> Pin 8, (-) -> GND
  Open the Serial Monitor at 9600 to see times and scores.
*/

const int GO_LED = 9;
const int P1_LED = 10;
const int P2_LED = 11;
const int P1_BTN = 2;
const int P2_BTN = 3;
const int BUZZER = 8;     // optional: without a buzzer, tone() is simply silent

int score1 = 0;
int score2 = 0;

bool pressed(int pin) { return digitalRead(pin) == LOW; }   // buttons use INPUT_PULLUP

// Celebrate a round: blink the winner's LED with rising beeps
void roundOver(int winner, unsigned long ms, bool foul) {
  if (winner == 1) score1++; else score2++;

  if (foul) {
    Serial.print("Too early! Player ");
    Serial.print(winner == 1 ? 2 : 1);
    Serial.println(" pressed before GO.");
  }
  Serial.print("Player ");
  Serial.print(winner);
  Serial.print(" wins");
  if (!foul) {
    Serial.print(" in ");
    Serial.print(ms);
    Serial.print(" ms");
  }
  Serial.println("!");
  Serial.print("Score  P1: ");
  Serial.print(score1);
  Serial.print("  P2: ");
  Serial.println(score2);

  int pin = (winner == 1) ? P1_LED : P2_LED;
  for (int i = 0; i < 5; i++) {
    digitalWrite(pin, HIGH);
    tone(BUZZER, 784 + i * 120, 80);
    delay(150);
    digitalWrite(pin, LOW);
    delay(100);
  }
  delay(1500);
}

void setup() {
  pinMode(GO_LED, OUTPUT);
  pinMode(P1_LED, OUTPUT);
  pinMode(P2_LED, OUTPUT);
  pinMode(P1_BTN, INPUT_PULLUP);
  pinMode(P2_BTN, INPUT_PULLUP);
  Serial.begin(9600);
  randomSeed(analogRead(A0));   // an unconnected analog pin gives random noise
  Serial.println("Reaction Duel! Wait for the GO light...");
}

void loop() {
  // Wait until both players let go of their buttons
  while (pressed(P1_BTN) || pressed(P2_BTN)) { }
  delay(500);

  Serial.println("Get ready...");
  tone(BUZZER, 440, 100);

  // Random wait of 2 to 6 seconds. Pressing now is a foul!
  unsigned long waitTime = random(2000, 6000);
  unsigned long start = millis();
  while (millis() - start < waitTime) {
    if (pressed(P1_BTN)) { roundOver(2, 0, true); return; }
    if (pressed(P2_BTN)) { roundOver(1, 0, true); return; }
  }

  // GO!
  digitalWrite(GO_LED, HIGH);
  tone(BUZZER, 1047, 150);
  unsigned long goTime = millis();

  while (true) {
    bool p1 = pressed(P1_BTN);
    bool p2 = pressed(P2_BTN);
    if (p1 || p2) {
      unsigned long reaction = millis() - goTime;
      digitalWrite(GO_LED, LOW);
      roundOver(p1 ? 1 : 2, reaction, false);   // a perfect tie goes to player 1
      return;
    }
    if (millis() - goTime > 5000) {
      digitalWrite(GO_LED, LOW);
      Serial.println("Too slow! Nobody pressed.");
      delay(1000);
      return;
    }
  }
}
