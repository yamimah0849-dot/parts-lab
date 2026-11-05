/*
  Plug-&-Play Traffic Light — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring (module has resistors built in): R -> Pin 4, Y -> Pin 3, G -> Pin 2, GND -> GND
*/

const int RED = 4, YELLOW = 3, GREEN = 2;

void lights(bool r, bool y, bool g) {
  digitalWrite(RED, r ? HIGH : LOW);
  digitalWrite(YELLOW, y ? HIGH : LOW);
  digitalWrite(GREEN, g ? HIGH : LOW);
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {
  lights(false, false, true);  delay(5000);   // green
  lights(false, true, false);  delay(2000);   // yellow
  lights(true, false, false);  delay(5000);   // red
  lights(true, true, false);   delay(1000);   // red + yellow: get ready
}
