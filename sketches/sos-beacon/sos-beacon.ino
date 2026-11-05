/*
  SOS Signal Beacon — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: LED long leg (+) -> 220 ohm resistor -> Pin 9
          LED short leg (-) -> GND
  Blinks "SOS" in Morse code on the external LED and on the board's built-in LED.
*/

const int LED_PIN = 9;     // external LED (through a 220 ohm resistor)
const int UNIT = 200;      // length of one Morse "dot" in milliseconds

// Turn both LEDs on or off together
void light(bool on) {
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
}

// A dot is 1 unit on, a dash is 3 units on. Each is followed by 1 unit off.
void dot()  { light(true); delay(UNIT);     light(false); delay(UNIT); }
void dash() { light(true); delay(UNIT * 3); light(false); delay(UNIT); }

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("SOS beacon started");
}

void loop() {
  Serial.print("S ");
  dot(); dot(); dot();
  delay(UNIT * 2);         // gap between letters is 3 units (1 already waited)

  Serial.print("O ");
  dash(); dash(); dash();
  delay(UNIT * 2);

  Serial.println("S");
  dot(); dot(); dot();
  delay(UNIT * 6);         // gap between words is 7 units
}
