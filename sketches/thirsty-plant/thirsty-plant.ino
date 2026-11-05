/*
  Thirsty Plant Alert — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Capacitive soil sensor: VCC -> 5V, GND -> GND, AOUT -> A0
          Green LED (+) -> 220 ohm -> Pin 5, Red LED (+) -> 220 ohm -> Pin 6, LED (-) -> GND
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
  Calibrate DRY_VALUE and WET_VALUE using the Serial Monitor (see the guide).
*/

const int SENSOR = A0;
const int GREEN = 5;
const int RED = 6;
const int BUZZER = 8;

int DRY_VALUE = 600;   // reading with the sensor in air      (CHANGE ME)
int WET_VALUE = 280;   // reading with the sensor in water    (CHANGE ME)

const int THIRSTY_PCT = 30;   // below this: red
const int HAPPY_PCT = 40;     // above this: green (in between keeps the last state)
const unsigned long CHIRP_EVERY_MS = 600000UL;   // 10 minutes

bool thirsty = false;
unsigned long lastChirp = 0;

void setup() {
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int raw = analogRead(SENSOR);      // lower = wetter for this sensor
  int pct = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  pct = constrain(pct, 0, 100);

  // Two thresholds ("hysteresis") stop the LEDs flickering near the limit
  if (pct < THIRSTY_PCT) thirsty = true;
  if (pct > HAPPY_PCT) thirsty = false;

  digitalWrite(GREEN, thirsty ? LOW : HIGH);
  digitalWrite(RED, thirsty ? HIGH : LOW);

  if (thirsty && (lastChirp == 0 || millis() - lastChirp > CHIRP_EVERY_MS)) {
    tone(BUZZER, 2400, 80); delay(120);
    tone(BUZZER, 2800, 80);
    lastChirp = millis();
  }
  if (!thirsty) lastChirp = 0;

  Serial.print("raw: ");
  Serial.print(raw);
  Serial.print("  moisture: ");
  Serial.print(pct);
  Serial.println(thirsty ? " %  -> water me!" : " %  -> happy");
  delay(1000);
}
