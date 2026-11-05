/*
  Wireless Doorbell — Parts Lab
  Boards: Arduino Uno / Nano (two of them)
  Library: rc-switch
  Sender:   433MHz transmitter DATA -> Pin 10 (VCC 5V, GND), button Pin 4 <-> GND
  Receiver: 433MHz receiver DATA -> Pin 2 (VCC 5V, GND), passive buzzer (+) -> 220 ohm -> Pin 8
  Set IS_SENDER before uploading to each board.
*/
#include <RCSwitch.h>

const bool IS_SENDER = true;           // true = button board, false = chime board
const unsigned long DOORBELL_CODE = 5393417;   // any number: both boards must match
const int BUTTON = 4, BUZZER = 8;

RCSwitch radio = RCSwitch();

void chime() {
  tone(BUZZER, 1319, 350); delay(400);   // ding (E6)
  tone(BUZZER, 1047, 600); delay(650);   // dong (C6)
}

void setup() {
  Serial.begin(9600);
  if (IS_SENDER) {
    pinMode(BUTTON, INPUT_PULLUP);
    radio.enableTransmit(10);
    radio.setRepeatTransmit(8);        // send each code several times for reliability
    Serial.println("Doorbell sender ready");
  } else {
    pinMode(BUZZER, OUTPUT);
    radio.enableReceive(0);            // interrupt 0 = Pin 2 on Uno/Nano
    Serial.println("Doorbell chime ready");
  }
}

void loop() {
  if (IS_SENDER) {
    if (digitalRead(BUTTON) == LOW) {
      Serial.println("Ding-dong! Sending...");
      radio.send(DOORBELL_CODE, 24);
      while (digitalRead(BUTTON) == LOW) delay(10);
      delay(300);
    }
  } else {
    if (radio.available()) {
      unsigned long value = radio.getReceivedValue();
      radio.resetAvailable();
      if (value == DOORBELL_CODE) {
        Serial.println("Doorbell code received");
        chime();
        radio.resetAvailable();        // ignore repeats that arrived during the chime
      } else {
        Serial.print("Other code ignored: ");
        Serial.println(value);
      }
    }
  }
}
