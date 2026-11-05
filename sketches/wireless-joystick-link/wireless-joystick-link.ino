/*
  Wireless Joystick Link — Parts Lab
  Board: Arduino Uno (x2)
  Libraries: RF24 (TMRh20), Servo
  Wiring (both boards): nRF24L01+: VCC -> 3.3V (+ 10uF capacitor across VCC/GND), GND -> GND,
                        CE -> 9, CSN -> 10, SCK -> 13, MOSI -> 11, MISO -> 12
  Transmitter: joystick VRx -> A0, VRy -> A1, +5V -> 5V, GND -> GND
  Receiver:    servo signal -> Pin 6, red -> 5V, brown -> GND
  Set IS_TRANSMITTER for each board before uploading.
*/
#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

const bool IS_TRANSMITTER = true;   // true for the joystick board, false for the servo board

RF24 radio(9, 10);                   // CE, CSN
const byte ADDRESS[6] = "PLAB1";     // both boards must use the same address

struct Packet {
  int16_t x;
  int16_t y;
};

Servo servo;
unsigned long lastPacket = 0;

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("nRF24 not responding: check wiring and 3.3V power");
    while (true) delay(100);
  }
  radio.setPALevel(RF24_PA_LOW);     // low power is plenty for a room and more stable
  if (IS_TRANSMITTER) {
    radio.openWritingPipe(ADDRESS);
    radio.stopListening();
    Serial.println("Transmitter ready");
  } else {
    servo.attach(6);
    servo.write(90);
    radio.openReadingPipe(1, ADDRESS);
    radio.startListening();
    Serial.println("Receiver ready");
  }
}

void loop() {
  if (IS_TRANSMITTER) {
    Packet p;
    p.x = analogRead(A0);
    p.y = analogRead(A1);
    bool ok = radio.write(&p, sizeof(p));
    if (!ok) Serial.println("no answer from receiver");
    delay(20);   // 50 packets per second
  } else {
    if (radio.available()) {
      Packet p;
      radio.read(&p, sizeof(p));
      lastPacket = millis();
      servo.write(map(p.x, 0, 1023, 0, 180));
      Serial.print("x=");
      Serial.print(p.x);
      Serial.print(" y=");
      Serial.println(p.y);
    }
    if (millis() - lastPacket > 1000) servo.write(90);   // failsafe: center if signal is lost
  }
}
