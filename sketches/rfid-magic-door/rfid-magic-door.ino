/*
  Magic Card Door Lock — Parts Lab
  Board: ESP32 DevKit
  Libraries: MFRC522, ESP32Servo
  Wiring: RC522: 3.3V -> 3V3, GND -> GND, SDA(SS) -> GPIO 5, SCK -> GPIO 18,
                 MOSI -> GPIO 23, MISO -> GPIO 19, RST -> GPIO 22 (IRQ not connected)
          Servo: red -> VIN (5V), brown -> GND, orange -> GPIO 13
          Optional: green LED -> 100 ohm -> GPIO 25, red LED -> 100 ohm -> GPIO 26
  The first card you tap becomes the key (saved in flash).
*/
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Preferences.h>

const int SS_PIN = 5, RST_PIN = 22, SERVO_PIN = 13, GREEN = 25, RED = 26, BOOT_BTN = 0;
const int LOCKED_ANGLE = 0, OPEN_ANGLE = 90;

MFRC522 rfid(SS_PIN, RST_PIN);
Servo latch;
Preferences prefs;      // small storage in the ESP32's flash memory
String keyUid = "";

String uidString() {    // card ID as text like "A1:B2:C3:D4"
  String s = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) s += "0";
    s += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) s += ":";
  }
  s.toUpperCase();
  return s;
}

void blink(int pin, int times) {
  for (int i = 0; i < times; i++) { digitalWrite(pin, HIGH); delay(120); digitalWrite(pin, LOW); delay(120); }
}

void setup() {
  Serial.begin(115200);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BOOT_BTN, INPUT_PULLUP);
  latch.attach(SERVO_PIN);
  latch.write(LOCKED_ANGLE);
  SPI.begin();             // default ESP32 pins: SCK 18, MISO 19, MOSI 23
  rfid.PCD_Init();

  prefs.begin("door", false);
  keyUid = prefs.getString("key", "");

  // 3-second window after start: hold BOOT for 2 s to forget the saved card
  unsigned long t0 = millis();
  while (millis() - t0 < 3000) {
    if (digitalRead(BOOT_BTN) == LOW) {
      unsigned long held = millis();
      while (digitalRead(BOOT_BTN) == LOW) {
        if (millis() - held > 2000) {
          prefs.remove("key");
          keyUid = "";
          Serial.println("Saved card forgotten.");
          blink(RED, 5);
          break;
        }
      }
    }
    delay(10);
  }
  Serial.println(keyUid == "" ? "No key yet: tap a card to make it the key." : "Locked. Tap the key card.");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
  String uid = uidString();
  rfid.PICC_HaltA();        // tell the card we're done
  rfid.PCD_StopCrypto1();
  Serial.print("Card: ");
  Serial.println(uid);

  if (keyUid == "") {       // first card becomes the key
    keyUid = uid;
    prefs.putString("key", uid);
    Serial.println("Saved as the key card!");
    blink(GREEN, 3);
    return;
  }
  if (uid == keyUid) {
    Serial.println("ACCESS GRANTED");
    digitalWrite(GREEN, HIGH);
    latch.write(OPEN_ANGLE);
    delay(3000);
    latch.write(LOCKED_ANGLE);
    digitalWrite(GREEN, LOW);
  } else {
    Serial.println("ACCESS DENIED");
    blink(RED, 4);
  }
  delay(500);
}
