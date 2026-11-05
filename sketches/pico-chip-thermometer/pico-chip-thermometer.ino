/*
  Chip Temperature Spy — Parts Lab
  Boards: Raspberry Pi Pico / Pico W (just the board!)
  Open Tools -> Serial Plotter at 115200 to see the graph.
*/

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  float tempC = analogReadTemp();      // the RP2040's built-in sensor
  Serial.print("chip_temp_C:");        // "name:value" makes a nice plot line
  Serial.println(tempC);

  // Warmer chip -> faster blinking (1000 ms at 20 C down to 100 ms at 40 C)
  int interval = map((int)(tempC * 10), 200, 400, 1000, 100);
  interval = constrain(interval, 100, 1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(interval / 2);
  digitalWrite(LED_BUILTIN, LOW);
  delay(interval / 2);
}
