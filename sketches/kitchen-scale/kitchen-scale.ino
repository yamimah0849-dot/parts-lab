/*
  Kitchen Scale — Parts Lab
  Boards: Arduino Uno / Nano
  Library: LiquidCrystal I2C
  Wiring: Load cell -> HX711 (red E+, black E-, white A-, green A+)
          HX711: VCC -> 5V, GND -> GND, DT -> Pin 3, SCK -> Pin 2
          LCD (I2C): VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
          Optional TARE button: Pin 4 <-> GND
*/
#include <LiquidCrystal_I2C.h>

const int HX_DT = 3, HX_SCK = 2, TARE_BTN = 4;
float CALIBRATION = 420.0;   // raw units per gram (CHANGE ME after calibrating)
long offset = 0;             // raw reading of the empty scale

LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if blank

// Read one 24-bit value from the HX711 (channel A, gain 128)
long readHX711() {
  unsigned long start = millis();
  while (digitalRead(HX_DT) == HIGH) {          // wait until data is ready
    if (millis() - start > 500) return 0;       // no HX711 answering
  }
  long value = 0;
  noInterrupts();                               // keep the clock timing tight
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX_SCK, HIGH);
    delayMicroseconds(1);
    value = (value << 1) | digitalRead(HX_DT);
    digitalWrite(HX_SCK, LOW);
    delayMicroseconds(1);
  }
  digitalWrite(HX_SCK, HIGH);                   // 25th pulse = channel A, gain 128 next time
  delayMicroseconds(1);
  digitalWrite(HX_SCK, LOW);
  interrupts();
  if (value & 0x800000) value |= 0xFF000000;    // make negative numbers work (24 -> 32 bit)
  return value;
}

long average(int n) {
  long sum = 0;
  for (int i = 0; i < n; i++) sum += readHX711();
  return sum / n;
}

void tare() {
  lcd.clear();
  lcd.print("Taring...");
  offset = average(15);
}

void setup() {
  pinMode(HX_SCK, OUTPUT);
  pinMode(HX_DT, INPUT);
  pinMode(TARE_BTN, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  delay(500);
  tare();
}

void loop() {
  if (digitalRead(TARE_BTN) == LOW) { tare(); while (digitalRead(TARE_BTN) == LOW) delay(5); }

  long raw = average(5) - offset;
  float grams = raw / CALIBRATION;
  if (abs(grams) < 0.5) grams = 0;             // hide tiny jitter around zero

  lcd.setCursor(0, 0);
  lcd.print("Weight:         ");
  lcd.setCursor(0, 1);
  lcd.print(grams, 0);
  lcd.print(" g          ");

  Serial.print("raw: ");
  Serial.print(raw);
  Serial.print("   grams: ");
  Serial.println(grams, 1);
  delay(200);
}
