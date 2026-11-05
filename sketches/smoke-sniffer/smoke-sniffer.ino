/*
  Smoke Sniffer (demo) — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  NOT a safety device: never use it instead of a real smoke/gas alarm.
  Wiring: MQ-2: VCC -> 5V, GND -> GND, A0 -> A0
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Optional red LED (+) -> 220 ohm -> Pin 13, (-) -> GND
*/

const int GAS = A0;
const int BUZZER = 8;
const int LED = 13;
const unsigned long WARMUP_MS = 60000;   // let the heater warm up first
const int RISE_TO_ALARM = 150;           // how far above clean air counts as "smoke"

int baseline = 0;

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Warming up the MQ-2 for 60 seconds...");
  while (millis() < WARMUP_MS) {
    digitalWrite(LED, (millis() / 500) % 2);   // slow blink while warming up
    if (millis() % 5000 < 20) {
      Serial.print((WARMUP_MS - millis()) / 1000);
      Serial.println(" s left");
      delay(20);
    }
  }
  digitalWrite(LED, LOW);

  // Measure "clean air" as the baseline
  long sum = 0;
  for (int i = 0; i < 50; i++) { sum += analogRead(GAS); delay(20); }
  baseline = sum / 50;
  Serial.print("Clean-air baseline: ");
  Serial.println(baseline);
  tone(BUZZER, 1500, 100);
}

void loop() {
  int value = analogRead(GAS);
  bool alarm = value > baseline + RISE_TO_ALARM;

  Serial.print("gas: ");
  Serial.print(value);
  Serial.println(alarm ? "  <-- ALARM" : "");

  if (alarm) {
    digitalWrite(LED, HIGH);
    tone(BUZZER, 2500, 200);
    delay(250);
    digitalWrite(LED, LOW);
    delay(150);
  } else {
    digitalWrite(LED, LOW);
    delay(300);
  }
}
