/*
  Secret Code Safe — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Libraries: Keypad (Mark Stanley), Servo
  Wiring: Keypad rows (pins 1-4) -> 9, 8, 7, 6 ; columns (pins 5-8) -> 5, 4, 3, 2
          Servo: brown -> GND, red -> 5V, orange -> Pin 10
          Optional: green LED -> 220 ohm -> 11, red LED -> 220 ohm -> 12, passive buzzer -> 220 ohm -> 13
  Type the code, then # to enter. * clears.
*/
#include <Keypad.h>
#include <Servo.h>

const char SECRET[] = "1234";    // CHANGE ME: your own code (digits only)
const int LOCKED_ANGLE = 0;
const int OPEN_ANGLE = 90;
const unsigned long OPEN_MS = 5000;

const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Servo lock;
const int SERVO_PIN = 10;
const int GREEN = 11, RED = 12, BUZZER = 13;

String typed = "";

void beep(int f, int ms) { tone(BUZZER, f, ms); }

void unlock() {
  Serial.println("  -> ACCESS GRANTED");
  digitalWrite(GREEN, HIGH);
  beep(1200, 100); delay(130); beep(1600, 150);
  lock.write(OPEN_ANGLE);
  delay(OPEN_MS);
  lock.write(LOCKED_ANGLE);
  digitalWrite(GREEN, LOW);
  Serial.println("Locked again");
}

void denied() {
  Serial.println("  -> WRONG CODE");
  digitalWrite(RED, HIGH);
  beep(200, 600);
  delay(800);
  digitalWrite(RED, LOW);
}

void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  lock.attach(SERVO_PIN);
  lock.write(LOCKED_ANGLE);
  Serial.begin(9600);
  Serial.println("Safe locked. Enter code + #");
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  beep(2000, 30);                  // key click
  if (key == '*') {
    typed = "";
    Serial.println(" (cleared)");
  } else if (key == '#') {
    Serial.println();
    if (typed == SECRET) unlock(); else denied();
    typed = "";
  } else if (typed.length() < 8) {
    typed += key;
    Serial.print('*');             // don't print the real digits
  }
}
