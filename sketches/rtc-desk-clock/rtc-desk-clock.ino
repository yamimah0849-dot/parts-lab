/*
  Always-Right Desk Clock — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: RTClib (Adafruit), TM1637 (Avishay Orpaz)
  Wiring: DS3231: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          TM1637: VCC -> 5V, GND -> GND, CLK -> Pin 3, DIO -> Pin 4
          Optional 12/24h button: Pin 2 <-> GND
*/
#include <RTClib.h>
#include <TM1637Display.h>

RTC_DS3231 rtc;
TM1637Display display(3, 4);   // CLK, DIO
const int MODE_BTN = 2;

// Set to true, upload once to copy your PC's time, then set back to false and upload again.
const bool FORCE_SET_TIME = false;
bool use24h = true;

const uint8_t GLYPH_DEGREE = SEG_A | SEG_B | SEG_F | SEG_G;   // the ° symbol
const uint8_t GLYPH_C = SEG_A | SEG_D | SEG_E | SEG_F;        // the letter C

void setup() {
  pinMode(MODE_BTN, INPUT_PULLUP);
  display.setBrightness(5);
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("DS3231 not found: check wiring");
    const uint8_t err[] = { SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, SEG_E | SEG_G, SEG_E | SEG_G, 0 };   // "Err"
    display.setSegments(err);
    while (true) delay(100);
  }
  if (rtc.lostPower() || FORCE_SET_TIME) {
    // __DATE__ and __TIME__ are the moment the code was compiled on your PC
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Clock set from the PC's time");
  }
}

void showTemperature() {
  int t = (int)round(rtc.getTemperature());   // the DS3231 has a thermometer inside
  uint8_t data[4] = {
    t >= 10 ? display.encodeDigit((t / 10) % 10) : (uint8_t)0,
    display.encodeDigit(abs(t) % 10),
    GLYPH_DEGREE,
    GLYPH_C
  };
  display.setSegments(data);
}

void loop() {
  if (digitalRead(MODE_BTN) == LOW) {
    use24h = !use24h;
    while (digitalRead(MODE_BTN) == LOW) delay(5);
    delay(30);
  }

  DateTime now = rtc.now();

  if (now.second() >= 30 && now.second() < 33) {   // show temperature for 3 s each minute
    showTemperature();
  } else {
    int h = now.hour();
    if (!use24h) { h = h % 12; if (h == 0) h = 12; }
    int value = h * 100 + now.minute();
    bool colon = now.second() % 2 == 0;            // blink once per second
    display.showNumberDecEx(value, colon ? 0b01000000 : 0, use24h);
  }

  static int lastSecond = -1;
  if (now.second() != lastSecond) {
    lastSecond = now.second();
    char buf[20];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    Serial.println(buf);
  }
  delay(100);
}
