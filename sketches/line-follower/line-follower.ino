/*
  Line-Following Racer — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Driver: TB6612FNG
  Wiring: PWMA -> 5, AIN1 -> 7, AIN2 -> 8, PWMB -> 6, BIN1 -> 9, BIN2 -> 10, STBY -> 4
          VM <- battery +, VCC <- 5V, all GND together (board, driver, battery -)
          Left TCRT5000 DO -> 2, right TCRT5000 DO -> 3 (both VCC -> 5V, GND -> GND)
*/

const int PWMA = 5, AIN1 = 7, AIN2 = 8;   // left motor
const int PWMB = 6, BIN1 = 9, BIN2 = 10;  // right motor
const int STBY = 4;
const int LEFT_SENSOR = 2, RIGHT_SENSOR = 3;

// Most TCRT5000 modules output HIGH over black (no reflection). Flip if yours is opposite.
const bool LINE_READS_HIGH = true;
const int FAST = 170, SLOW = 60;
int lastTurn = 0;   // -1 = last saw the line on the left, 1 = right

bool onLine(int pin) { return (digitalRead(pin) == HIGH) == LINE_READS_HIGH; }

void motor(int pwm, int a, int b, int speed) {
  if (speed >= 0) { digitalWrite(a, HIGH); digitalWrite(b, LOW); }
  else            { digitalWrite(a, LOW);  digitalWrite(b, HIGH); speed = -speed; }
  analogWrite(pwm, constrain(speed, 0, 255));
}
void drive(int left, int right) {
  motor(PWMA, AIN1, AIN2, left);
  motor(PWMB, BIN1, BIN2, right);
}

void setup() {
  int pins[] = {PWMA, AIN1, AIN2, PWMB, BIN1, BIN2, STBY};
  for (int p : pins) pinMode(p, OUTPUT);
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  digitalWrite(STBY, HIGH);   // wake the driver up
  drive(0, 0);
  delay(2000);
}

void loop() {
  bool left = onLine(LEFT_SENSOR);
  bool right = onLine(RIGHT_SENSOR);

  if (left && right) {            // both see the line (crossing): go straight
    drive(FAST, FAST);
  } else if (left) {              // line is to the left: steer left
    drive(SLOW, FAST);
    lastTurn = -1;
  } else if (right) {             // line is to the right: steer right
    drive(FAST, SLOW);
    lastTurn = 1;
  } else {                        // between the sensors... or lost
    if (lastTurn == 0) drive(FAST, FAST);
    else if (lastTurn < 0) drive(-SLOW, FAST);   // search toward where we last saw it
    else drive(FAST, -SLOW);
  }
}
