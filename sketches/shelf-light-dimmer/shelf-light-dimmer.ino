/*
  12V Shelf Light Dimmer — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: 12V adapter -> barrel jack breakout. +12V -> strip (+), strip (-) -> MOSFET Drain
          MOSFET Source -> 12V GND and board GND (common ground)
          MOSFET Gate -> 220 ohm -> Pin 9, plus 10k ohm Gate -> GND
          Potentiometer: outer legs -> 5V and GND, middle -> A0
          The board itself runs from USB.
*/

const int MOSFET = 9;   // PWM pin
const int KNOB = A0;
float level = 0;

void setup() {
  pinMode(MOSFET, OUTPUT);
  analogWrite(MOSFET, 0);
  Serial.begin(9600);
  // Gentle fade-in at power-up up to the knob's position
  int target = analogRead(KNOB) / 4;
  for (int v = 0; v <= target; v++) { analogWrite(MOSFET, v); delay(6); }
  level = target;
}

void loop() {
  int target = analogRead(KNOB) / 4;      // 0..255
  if (target < 4) target = 0;             // the bottom of the knob = fully off
  level = level * 0.9 + target * 0.1;     // smooth changes
  analogWrite(MOSFET, (int)level);
  Serial.println((int)level);
  delay(20);
}
