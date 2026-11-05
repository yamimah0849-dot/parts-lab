/*
  Bluetooth Lamp (Android) — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: HC-05: VCC -> 5V, GND -> GND, TXD -> Pin 10
          Pin 11 -> 1k ohm -> HC-05 RXD, and 2k ohm from HC-05 RXD -> GND (voltage divider)
          LED (+) -> 220 ohm -> Pin 9, (-) -> GND
  Phone (Android): pair "HC-05" (PIN 1234 or 0000), use a serial terminal app, send 0-9.
*/
#include <SoftwareSerial.h>

SoftwareSerial bt(10, 11);   // RX (from HC-05 TXD), TX (to HC-05 RXD via divider)
const int LED = 9;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  bt.begin(9600);            // HC-05 default speed in data mode
  Serial.println("Waiting for Bluetooth commands 0-9...");
}

void loop() {
  if (bt.available()) {
    char c = bt.read();
    if (c >= '0' && c <= '9') {
      int level = c - '0';                       // 0..9
      analogWrite(LED, map(level, 0, 9, 0, 255));
      String reply = "Brightness: " + String(level * 100 / 9) + "%";
      bt.println(reply);                          // answer the phone
      Serial.println(reply);
    }
  }
}
