/*
  Tea & Coffee Thermometer — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Libraries: OneWire, DallasTemperature, TM1637
  Wiring: DS18B20 probe: red -> 5V, black -> GND, yellow -> Pin 2, 4.7k ohm between Pin 2 and 5V
          TM1637: VCC -> 5V, GND -> GND, CLK -> Pin 3, DIO -> Pin 4
          Optional passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM1637Display.h>

OneWire oneWire(2);
DallasTemperature probe(&oneWire);
TM1637Display display(3, 4);
const int BUZZER = 8;

const float HOT_C = 65;     // drink counts as "hot" above this
const float DRINK_C = 60;   // beep when it cools below this
bool wasHot = false;

const uint8_t GLYPH_DEGREE = SEG_A | SEG_B | SEG_F | SEG_G;
const uint8_t GLYPH_C = SEG_A | SEG_D | SEG_E | SEG_F;
const uint8_t GLYPH_DASH = SEG_G;

void showTemp(int t) {
  uint8_t data[4];
  data[0] = (t >= 10) ? display.encodeDigit((t / 10) % 10) : 0;
  data[1] = display.encodeDigit(t % 10);
  data[2] = GLYPH_DEGREE;
  data[3] = GLYPH_C;
  display.setSegments(data);
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  probe.begin();
  display.setBrightness(5);
}

void loop() {
  probe.requestTemperatures();
  float t = probe.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) {
    const uint8_t dashes[] = {GLYPH_DASH, GLYPH_DASH, GLYPH_DASH, GLYPH_DASH};
    display.setSegments(dashes);
    Serial.println("Probe not found: check wiring and the 4.7k resistor");
    delay(1000);
    return;
  }
  showTemp(constrain((int)(t + 0.5), 0, 99));
  Serial.print(t, 1);
  Serial.println(" C");

  if (t > HOT_C) wasHot = true;
  if (wasHot && t < DRINK_C) {               // cooled to drinking temperature!
    for (int i = 0; i < 3; i++) { tone(BUZZER, 1760, 120); delay(220); }
    wasHot = false;
  }
  delay(750);
}
