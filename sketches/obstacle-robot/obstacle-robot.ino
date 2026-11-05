/*
  Obstacle-Dodging Robot — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Driver: L298N (5V-EN, ENA and ENB jumpers REMOVED)
  Wiring: ENA -> 5, IN1 -> 7, IN2 -> 8, IN3 -> 9, IN4 -> 10, ENB -> 6
          Ultrasonic: VCC -> 5V, GND -> GND, TRIG -> 11, ECHO -> 12
          L298N 12V <- battery +, L298N GND <- battery - AND board GND, L298N 5V <- board 5V
*/

const int ENA = 5, IN1 = 7, IN2 = 8, IN3 = 9, IN4 = 10, ENB = 6;
const int TRIG = 11, ECHO = 12;
const int TURN_CM = 25;       // closer than this = obstacle
const int CRUISE = 170;       // forward speed (0..255)

void motor(int en, int a, int b, int speed) {
  if (speed >= 0) { digitalWrite(a, HIGH); digitalWrite(b, LOW); }
  else            { digitalWrite(a, LOW);  digitalWrite(b, HIGH); speed = -speed; }
  analogWrite(en, constrain(speed, 0, 255));
}
void drive(int left, int right) {
  motor(ENA, IN1, IN2, left);
  motor(ENB, IN3, IN4, right);
}

long distanceCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long us = pulseIn(ECHO, HIGH, 25000);   // max ~4 m
  if (us == 0) return 999;                // no echo = nothing close
  return us / 58;
}

void setup() {
  int pins[] = {ENA, IN1, IN2, IN3, IN4, ENB, TRIG};
  for (int p : pins) pinMode(p, OUTPUT);
  pinMode(ECHO, INPUT);
  drive(0, 0);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  delay(3000);                            // time to put it on the floor
}

void loop() {
  long cm = distanceCm();
  if (cm < TURN_CM) {
    Serial.print("Obstacle at ");
    Serial.print(cm);
    Serial.println(" cm: dodging!");
    drive(0, 0);            delay(150);
    drive(-160, -160);      delay(350);   // back up
    int dir = random(2) == 0 ? 1 : -1;    // pick left or right
    drive(170 * dir, -170 * dir);
    delay(random(300, 650));              // turn for a random time
    drive(0, 0);            delay(100);
  } else {
    drive(CRUISE, CRUISE);
  }
  delay(50);
}
