/*
  Auto Plant Waterer — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Soil sensor: VCC -> 5V, GND -> GND, AOUT -> A0
          MOSFET IRLZ44N: Gate -> 220 ohm -> Pin 7, 10k ohm Gate -> GND, Source -> GND, Drain -> pump (-)
          Pump (+) -> 5V, 1N4007 across the pump (stripe toward 5V)
          Optional 100uF capacitor between 5V and GND
*/

const int SENSOR = A0;
const int PUMP = 7;

int DRY_VALUE = 600;   // sensor in air      (CHANGE ME after calibrating)
int WET_VALUE = 280;   // sensor in water    (CHANGE ME)

const int WATER_BELOW_PCT = 35;           // water when moisture is below this
const unsigned long PUMP_MS = 3000;       // one burst of water
const unsigned long SOAK_MS = 60000;      // wait for the water to soak in
const int MAX_BURSTS = 5;                 // safety limit in a row

int bursts = 0;

int moisturePct() {
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(SENSOR); delay(20); }
  int pct = map(sum / 10, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(pct, 0, 100);
}

void setup() {
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Auto Plant Waterer ready");
}

void loop() {
  int pct = moisturePct();
  Serial.print("Moisture: ");
  Serial.print(pct);
  Serial.println(" %");

  if (pct < WATER_BELOW_PCT) {
    if (bursts < MAX_BURSTS) {
      Serial.println("Dry! Watering...");
      digitalWrite(PUMP, HIGH);
      delay(PUMP_MS);
      digitalWrite(PUMP, LOW);
      bursts++;
      Serial.println("Waiting for the water to soak in...");
      delay(SOAK_MS);
    } else {
      // Something is wrong (sensor out of the soil? tank empty?): stop and blink
      Serial.println("SAFETY STOP: watered 5 times and still dry. Check the sensor and the tank, then press RESET.");
      while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(200); digitalWrite(LED_BUILTIN, LOW); delay(200); }
    }
  } else {
    bursts = 0;   // soil is fine again
    delay(5000);
  }
}
