/*
  IR Remote Copier — Parts Lab
  Boards: Arduino Uno / Nano
  Library: IRremote (version 4)
  Wiring: IR receiver: OUT -> Pin 2, + -> 5V, - -> GND
          IR LED (+) -> 220 ohm -> Pin 3 (fixed by the library on Uno/Nano), (-) -> GND
          Button: Pin 4 <-> GND (hold 2 s = learn, tap = send)
*/
#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 2;
const int IR_LED_PIN = 3;
const int BUTTON = 4;

IRData learned;          // the copied code
bool haveCode = false;
bool learning = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  IrSender.begin(IR_LED_PIN);
  Serial.println("Hold the button 2 s to learn a remote button; tap to send it.");
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    unsigned long t = millis();
    while (digitalRead(BUTTON) == LOW && millis() - t < 2000) delay(5);
    if (millis() - t >= 2000) {
      learning = true;
      Serial.println("LEARNING: press a button on your remote now...");
      while (digitalRead(BUTTON) == LOW) delay(5);
    } else if (haveCode) {
      Serial.println("Sending learned code");
      IrSender.write(&learned, 1);       // send it (plus one repeat)
      IrReceiver.restartAfterSend();     // listen again
    } else {
      Serial.println("Nothing learned yet: hold the button for 2 s first.");
    }
    delay(50);
  }

  if (IrReceiver.decode()) {
    bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
    if (learning && !isRepeat && IrReceiver.decodedIRData.protocol != UNKNOWN) {
      learned = IrReceiver.decodedIRData;
      haveCode = true;
      learning = false;
      Serial.print("Learned ");
      Serial.print(getProtocolString(learned.protocol));
      Serial.print(" address 0x");
      Serial.print(learned.address, HEX);
      Serial.print(" command 0x");
      Serial.println(learned.command, HEX);
    }
    IrReceiver.resume();
  }
}
