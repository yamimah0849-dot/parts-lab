/*
  Speedrun Stopwatch — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: TM1637 (by Avishay Orpaz)
  Wiring: TM1637: VCC -> 5V, GND -> GND, CLK -> Pin 3, DIO -> Pin 4
          START/STOP button: Pin 2 <-> GND
          RESET button: Pin 5 <-> GND
*/
#include <TM1637Display.h>

const int CLK = 3;
const int DIO = 4;
const int START_BTN = 2;
const int RESET_BTN = 5;

TM1637Display display(CLK, DIO);

bool running = false;
unsigned long startedAt = 0;   // millis() when the current run started
unsigned long elapsed = 0;     // time banked from earlier runs (ms)

bool pressed(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(25);                              // debounce
    if (digitalRead(pin) == LOW) {
      while (digitalRead(pin) == LOW) delay(5);
      return true;
    }
  }
  return false;
}

void showTime(unsigned long ms, bool colonOn) {
  int value;
  if (ms < 60000UL) {                       // SS:hh  (seconds and hundredths)
    value = (ms / 1000) * 100 + (ms % 1000) / 10;
  } else {                                  // MM:SS
    unsigned long s = ms / 1000;
    unsigned long m = s / 60;
    if (m > 99) m = 99;
    value = m * 100 + s % 60;
  }
  display.showNumberDecEx(value, colonOn ? 0b01000000 : 0, true);
}

void setup() {
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(RESET_BTN, INPUT_PULLUP);
  display.setBrightness(7);                 // 0 (dim) .. 7 (bright)
  showTime(0, true);
  Serial.begin(9600);
}

void loop() {
  if (pressed(START_BTN)) {
    if (running) {
      elapsed += millis() - startedAt;      // stop: bank the time
      running = false;
      Serial.print("Time: ");
      Serial.print(elapsed / 1000.0, 2);
      Serial.println(" s");
    } else {
      startedAt = millis();                 // start
      running = true;
    }
  }
  if (!running && pressed(RESET_BTN)) {
    elapsed = 0;
    Serial.println("Reset");
  }

  unsigned long now = elapsed + (running ? millis() - startedAt : 0);
  bool colon = running ? (millis() / 250) % 2 == 0 : true;   // blink while running
  showTime(now, colon);
  delay(10);
}
