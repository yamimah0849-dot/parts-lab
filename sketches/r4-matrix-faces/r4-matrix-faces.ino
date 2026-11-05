/*
  Uno R4 Matrix Mood Faces — Parts Lab
  Board: Arduino Uno R4 WiFi (just the board!)
  Uses the built-in 12x8 LED matrix. Type happy, sad, wow, love or sleep in the Serial Monitor (115200, Newline).
*/
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Each face: 8 rows of 12 characters ('#' = LED on)
const char* FACES[5][8] = {
  { "            ", "  ##    ##  ", "  ##    ##  ", "            ", " #        # ", "  #      #  ", "   ######   ", "            " },   // happy
  { "            ", "  ##    ##  ", "  ##    ##  ", "            ", "            ", "   ######   ", "  #      #  ", " #        # " },   // sad
  { "            ", " ###    ### ", " ###    ### ", "            ", "     ##     ", "    #  #    ", "     ##     ", "            " },   // wow
  { "            ", "  ###  ###  ", " ########## ", " ########## ", "  ########  ", "   ######   ", "    ####    ", "     ##     " },   // love
  { "          z ", "         z  ", " ###    ### ", "            ", "            ", "    ####    ", "            ", "            " }    // sleep
};
const char* NAMES[5] = {"happy", "sad", "wow", "love", "sleep"};
int face = 0;
uint8_t frame[8][12];

void show(int f, bool blink) {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 12; c++) {
      bool on = FACES[f][r][c] != ' ';
      if (blink && f != 3 && f != 4 && (r == 1 || r == 2)) on = false;   // eyes shut
      if (blink && f != 3 && f != 4 && r == 2 && FACES[f][1][c] != ' ') on = true;
      frame[r][c] = on ? 1 : 0;
    }
  }
  matrix.renderBitmap(frame, 8, 12);
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  show(face, false);
  Serial.println("Type: happy, sad, wow, love, sleep");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
    for (int i = 0; i < 5; i++) if (cmd == NAMES[i]) { face = i; Serial.print("Mood: "); Serial.println(NAMES[i]); }
    show(face, false);
  }
  static unsigned long nextBlink = 3000;
  if (millis() > nextBlink) {
    show(face, true);
    delay(150);
    show(face, false);
    nextBlink = millis() + random(2500, 6000);
  }
}
