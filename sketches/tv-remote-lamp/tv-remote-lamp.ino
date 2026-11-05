/*
  TV Remote Color Lamp — Parts Lab
  Boards: Arduino Uno / Nano
  Library: IRremote (version 4)
  Wiring: IR receiver: OUT -> Pin 2, + -> 5V, - -> GND
          RGB LED (common cathode, long leg -> GND), each color through 220 ohm:
          red -> Pin 9, green -> Pin 10, blue -> Pin 6
  (Pins 3 and 11 are avoided because the IR library uses their timer.)
*/
#include <IRremote.hpp>

const int IR_PIN = 2;
const int RED = 9, GREEN = 10, BLUE = 6;

// red, orange, yellow, green, cyan, blue, purple, pink, white, off
const byte COLORS[10][3] = {
  {255, 0, 0}, {255, 80, 0}, {255, 200, 0}, {0, 255, 0}, {0, 255, 200},
  {0, 0, 255}, {140, 0, 255}, {255, 40, 120}, {255, 255, 255}, {0, 0, 0}
};
int colorIndex = 9;   // start "off"

void setColor(int i) {
  analogWrite(RED, COLORS[i][0]);
  analogWrite(GREEN, COLORS[i][1]);
  analogWrite(BLUE, COLORS[i][2]);
}

void setup() {
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  setColor(colorIndex);
  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);
  Serial.println("Point a remote at the receiver and press a button");
}

void loop() {
  if (IrReceiver.decode()) {
    bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
    if (!isRepeat) {
      Serial.print("Button code: 0x");
      Serial.println(IrReceiver.decodedIRData.command, HEX);
      colorIndex = (colorIndex + 1) % 10;   // any new press = next color
      setColor(colorIndex);
    }
    IrReceiver.resume();   // ready for the next code
  }
}
