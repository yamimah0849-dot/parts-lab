/*
  Joystick Mouse — Parts Lab
  Board: Arduino Leonardo (it can act as a USB mouse)
  Library: Mouse
  Wiring: Joystick: GND -> GND, +5V -> 5V, VRx -> A0, VRy -> A1, SW -> Pin 2
  Click the stick once to enable. Hold it 2 s to disable.
*/
#include <Mouse.h>

const int X_PIN = A0, Y_PIN = A1, SW_PIN = 2;
const bool INVERT_Y = false;     // set true if up/down feels backwards
const int DEAD_ZONE = 40;        // ignore tiny movements around the center

bool enabled = false;
bool pressing = false;
unsigned long pressStart = 0;
bool longHandled = false;

int axis(int raw) {
  int c = raw - 512;              // center the reading around 0
  if (abs(c) < DEAD_ZONE) return 0;
  return c / 64;                  // up to about +/- 8 pixels per step
}

void setup() {
  pinMode(SW_PIN, INPUT_PULLUP);
}

void loop() {
  bool down = digitalRead(SW_PIN) == LOW;
  if (down && !pressing) { pressing = true; pressStart = millis(); longHandled = false; }
  if (down && pressing && !longHandled && millis() - pressStart > 2000) {
    longHandled = true;
    if (enabled) { enabled = false; Mouse.end(); }            // long press: switch off
  }
  if (!down && pressing) {
    pressing = false;
    if (!longHandled) {
      if (!enabled) { enabled = true; Mouse.begin(); }        // first click switches on
      else Mouse.click(MOUSE_LEFT);
    }
    delay(30);
  }

  if (enabled) {
    int dx = axis(analogRead(X_PIN));
    int dy = axis(analogRead(Y_PIN));
    if (INVERT_Y) dy = -dy;
    if (dx != 0 || dy != 0) Mouse.move(dx, dy, 0);
  }
  delay(10);
}
