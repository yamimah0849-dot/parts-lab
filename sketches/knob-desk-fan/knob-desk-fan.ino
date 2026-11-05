/*
  Knob-Speed Desk Fan — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: MOSFET IRLZ44N: Gate -> 220 ohm -> Pin 9, 10k ohm Gate -> GND, Source -> GND
          Motor: one wire -> battery +, other wire -> MOSFET Drain
          1N4007 across the motor, stripe toward battery +
          Battery - -> GND (common ground)
          Potentiometer: outer legs -> 5V and GND, middle -> A0
*/

const int FAN = 9;          // PWM pin to the MOSFET gate
const int KNOB = A0;
const int MIN_PWM = 60;     // below this the motor just hums, so start here

void setup() {
  pinMode(FAN, OUTPUT);
  analogWrite(FAN, 0);
  Serial.begin(9600);
}

void loop() {
  int knob = analogRead(KNOB);
  int pwm = 0;
  if (knob > 50) {                          // a small "off" zone at the start of the knob
    pwm = map(knob, 50, 1023, MIN_PWM, 255);
  }
  analogWrite(FAN, pwm);
  Serial.print("Fan speed: ");
  Serial.print(pwm * 100 / 255);
  Serial.println(" %");
  delay(100);
}
