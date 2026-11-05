/*
  Gamer Macro Pad — Parts Lab
  Board: Arduino Leonardo (it can act as a USB keyboard)
  Library: Keyboard
  Wiring: Buttons between Pins 2, 3, 4, 5 and GND (internal pull-ups)
*/
#include <Keyboard.h>

const int BUTTONS[4] = {2, 3, 4, 5};
bool wasDown[4] = {false, false, false, false};

// Change these to whatever your apps or games use!
void sendShortcut(int i) {
  switch (i) {
    case 0: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('c'); break;   // copy
    case 1: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('v'); break;   // paste
    case 2: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('z'); break;   // undo
    case 3: Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('s'); break;   // Windows snip
  }
  delay(40);
  Keyboard.releaseAll();
}

void setup() {
  for (int i = 0; i < 4; i++) pinMode(BUTTONS[i], INPUT_PULLUP);
  delay(3000);          // safety pause after plugging in
  Keyboard.begin();
}

void loop() {
  for (int i = 0; i < 4; i++) {
    bool down = digitalRead(BUTTONS[i]) == LOW;
    if (down && !wasDown[i]) sendShortcut(i);   // only on a NEW press
    wasDown[i] = down;
  }
  delay(10);   // simple debounce
}
