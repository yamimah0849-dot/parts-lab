/*
  Dancing Robot Car — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Driver: L298N (5V-EN, ENA and ENB jumpers REMOVED)
  Wiring: ENA -> 5, IN1 -> 7, IN2 -> 8, IN3 -> 9, IN4 -> 10, ENB -> 6
          L298N 12V <- battery +, L298N GND <- battery - AND board GND, L298N 5V <- board 5V
          Left motor -> OUT1/OUT2, right motor -> OUT3/OUT4
*/

const int ENA = 5, IN1 = 7, IN2 = 8;    // left motor
const int IN3 = 9, IN4 = 10, ENB = 6;   // right motor

// Drive one motor: speed from -255 (full reverse) to 255 (full forward)
void motor(int en, int a, int b, int speed) {
  if (speed >= 0) { digitalWrite(a, HIGH); digitalWrite(b, LOW); }
  else            { digitalWrite(a, LOW);  digitalWrite(b, HIGH); speed = -speed; }
  analogWrite(en, constrain(speed, 0, 255));
}

void drive(int left, int right, int ms) {
  motor(ENA, IN1, IN2, left);
  motor(ENB, IN3, IN4, right);
  delay(ms);
}

void stopAll() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void setup() {
  int pins[] = {ENA, IN1, IN2, IN3, IN4, ENB};
  for (int p : pins) pinMode(p, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  stopAll();
  Serial.begin(9600);
  // 3-second countdown so you can put the robot down
  for (int i = 0; i < 6; i++) { digitalWrite(LED_BUILTIN, i % 2 == 0); delay(500); }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  Serial.println("Dance!");
  drive(200, 200, 800);     // forward
  drive(-180, 180, 700);    // spin left
  drive(180, -180, 700);    // spin right
  for (int i = 0; i < 4; i++) {   // wiggle
    drive(200, 0, 180);
    drive(0, 200, 180);
  }
  drive(-170, -170, 600);   // back up
  drive(255, 255, 600);     // victory zoom
  drive(220, -220, 1200);   // big spin
  stopAll();
  Serial.println("Pose! (resting 5 s)");
  delay(5000);
}
