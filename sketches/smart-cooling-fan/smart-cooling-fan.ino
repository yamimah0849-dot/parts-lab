/*
  Smart Cooling Fan — Parts Lab
  Boards: Arduino Uno / Mega / Uno R4 (powered by a 12V adapter in the barrel jack)
  Libraries: DHT sensor library, Adafruit Unified Sensor
  Wiring: Fan (+) -> VIN (12V), fan (-) -> MOSFET Drain, 1N4007 across the fan (stripe to VIN)
          MOSFET Source -> GND, Gate -> 220 ohm -> Pin 9, 10k ohm Gate -> GND
          DHT11 module: + -> 5V, - -> GND, DATA -> Pin 2
*/
#include <DHT.h>

DHT dht(2, DHT11);
const int FAN = 9;
const float ON_C = 28.0;    // fan on above this
const float OFF_C = 26.0;   // fan off below this (the gap stops on/off flickering)
bool fanOn = false;

void setup() {
  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("DHT11 read failed");
  } else {
    if (!fanOn && t > ON_C) fanOn = true;
    if (fanOn && t < OFF_C) fanOn = false;
    digitalWrite(FAN, fanOn ? HIGH : LOW);   // simple on/off avoids PWM humming in PC fans
    Serial.print(t, 1);
    Serial.print(" C  fan ");
    Serial.println(fanOn ? "ON" : "OFF");
  }
  delay(2000);
}
