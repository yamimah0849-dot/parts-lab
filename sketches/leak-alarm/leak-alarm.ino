/*
  Water Leak Alarm — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo / Uno R4 / Nano Every
  Wiring: Water sensor: + -> Pin 7 (powered only while measuring), - -> GND, S -> A0
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Optional red LED (+) -> 220 ohm -> Pin 13, (-) -> GND
*/

const int SENSOR_POWER = 7;
const int SENSOR = A0;
const int BUZZER = 8;
const int LED = 13;
const int WET_LEVEL = 100;   // readings above this count as water

int readSensor() {
  digitalWrite(SENSOR_POWER, HIGH);   // power on
  delay(10);                          // let it settle
  int v = analogRead(SENSOR);
  digitalWrite(SENSOR_POWER, LOW);    // power off again (prevents corrosion)
  return v;
}

void setup() {
  pinMode(SENSOR_POWER, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int v = readSensor();
  Serial.print("water: ");
  Serial.println(v);
  if (v > WET_LEVEL) {
    for (int i = 0; i < 4; i++) {     // urgent beeping for about 2 seconds
      digitalWrite(LED, HIGH);
      tone(BUZZER, 3000, 200);
      delay(250);
      digitalWrite(LED, LOW);
      delay(250);
    }
  } else {
    delay(2000);                      // check every 2 seconds
  }
}
