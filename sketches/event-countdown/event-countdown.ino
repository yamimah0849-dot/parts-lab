/*
  Event Countdown — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: RTClib, TM1637
  Wiring: DS3231: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          TM1637: VCC -> 5V, GND -> GND, CLK -> Pin 3, DIO -> Pin 4
          Optional button: Pin 2 <-> GND (hold to show hours left)
*/
#include <RTClib.h>
#include <TM1637Display.h>

// CHANGE ME: the date you are counting down to
const int TARGET_YEAR = 2027;
const int TARGET_MONTH = 3;
const int TARGET_DAY = 10;

RTC_DS3231 rtc;
TM1637Display display(3, 4);
const int BUTTON = 2;
const uint8_t DASHES[] = {SEG_G, SEG_G, SEG_G, SEG_G};

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  display.setBrightness(5);
  Serial.begin(9600);
  if (!rtc.begin()) { Serial.println("DS3231 not found"); display.setSegments(DASHES); while (true) delay(100); }
  if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();
  DateTime today(now.year(), now.month(), now.day());
  DateTime target(TARGET_YEAR, TARGET_MONTH, TARGET_DAY);

  if (today.unixtime() > target.unixtime()) {          // the day has passed
    display.setSegments(DASHES);
  } else if (today.unixtime() == target.unixtime()) {  // it's today! flash 0000
    if (now.second() % 2 == 0) display.showNumberDec(0, true);
    else display.clear();
  } else {
    TimeSpan left = target - now;
    if (digitalRead(BUTTON) == LOW) {
      long hours = left.days() * 24L + left.hours();
      display.showNumberDec(min(hours, 9999L), false);
    } else {
      display.showNumberDec(left.days() + 1, false);   // +1: today counts as a day left
    }
  }
  delay(200);
}
