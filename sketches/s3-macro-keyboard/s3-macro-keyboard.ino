/*
  ESP32-S3 Macro Keyboard — Parts Lab
  Board: ESP32-S3 DevKitC-1   (Tools: USB Mode -> "USB-OTG (TinyUSB)")
  Wiring: Buttons between GPIO 4, 5, 6, 7 and GND (internal pull-ups)
  Upload through the UART port, then plug the USB port into your PC: it's now a keyboard.
*/
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;
const int BUTTONS[4] = {4, 5, 6, 7};
bool wasDown[4] = {false, false, false, false};

void sendShortcut(int i) {
  switch (i) {
    case 0: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('c'); break;                               // copy
    case 1: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('v'); break;                               // paste
    case 2: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('m'); break; // mute mic
    case 3: Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('s'); break;  // screenshot
  }
  delay(40);
  Keyboard.releaseAll();
}

void setup() {
  for (int i = 0; i < 4; i++) pinMode(BUTTONS[i], INPUT_PULLUP);
  Keyboard.begin();
  USB.begin();
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool down = digitalRead(BUTTONS[i]) == LOW;
    if (down && !wasDown[i]) sendShortcut(i);
    wasDown[i] = down;
  }
  delay(10);
}
