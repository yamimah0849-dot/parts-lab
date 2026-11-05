/*
  RGB Emoji Matrix — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: Adafruit NeoPixel
  Wiring: WS2812B 8x8 matrix: 5V -> 5V, GND -> GND, DIN -> 330 ohm -> Pin 6
          Button: Pin 2 <-> GND
*/
#include <Adafruit_NeoPixel.h>
#include <avr/pgmspace.h>

const int PIN = 6, BUTTON = 2;
const uint8_t BRIGHTNESS = 20;   // ~8%: safe for USB power. Raise only with a 5V 4A supply.
const bool SERPENTINE = false;   // set true if every other row looks mirrored

Adafruit_NeoPixel matrix(64, PIN, NEO_GRB + NEO_KHZ800);

// Original 8x8 pixel art. Letters pick colors from the palette below ('.' = off).
const char ART[6][65] PROGMEM = {
  ".rr..rr.rrrrrrrrrrrrrrrrrrrrrrrr.rrrrrr...rrrr.....rr...........",   // heart
  "..yyyy...yyyyyy.yykyykyyyyyyyyyyykyyyykyyykkkkyy.yyyyyy...yyyy..",   // smiley
  "...cc.....cccc...cccccc.cckcckcccccccccccccccccc.cccccc.........",   // slime
  "..wwww...wwwwww.wwkwwkwwwwkwwkwwwwwwwwwwwwwwwwwwwwwwwwwww.ww.ww.",   // ghost
  "...yy......yy...yyyyyyyy.yyyyyy...yyyy...yy..yy.yy....yy........",   // star
  "..g..g.....gg...gggggggg.gkggkg.gggggggggggggggg.g....g...g..g..",   // alien
};
const char* const NAMES[6] = {"heart", "smiley", "slime", "ghost", "star", "alien"};
int current = 0;

uint32_t colorFor(char c) {
  switch (c) {
    case 'r': return matrix.Color(255, 20, 60);
    case 'y': return matrix.Color(255, 200, 0);
    case 'c': return matrix.Color(0, 230, 180);
    case 'w': return matrix.Color(220, 220, 255);
    case 'g': return matrix.Color(40, 255, 60);
    case 'k': return matrix.Color(0, 0, 40);     // dark eye pixels
    default:  return 0;
  }
}

int indexOf(int x, int y) {
  if (SERPENTINE && (y % 2 == 1)) return y * 8 + (7 - x);
  return y * 8 + x;
}

void show(int n) {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      char c = pgm_read_byte(&ART[n][y * 8 + x]);
      matrix.setPixelColor(indexOf(x, y), colorFor(c));
    }
  }
  matrix.show();
  Serial.println(NAMES[n]);
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  show(current);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    current = (current + 1) % 6;
    show(current);
    while (digitalRead(BUTTON) == LOW) delay(5);
    delay(40);
  }
}
