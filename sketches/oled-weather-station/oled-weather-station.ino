/*
  Mini Weather Station — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: DHT sensor library, Adafruit Unified Sensor, Adafruit SSD1306, Adafruit GFX Library
  Wiring: DHT22: + -> 5V, - -> GND, DATA -> Pin 2
          OLED 128x64 (SSD1306, I2C): VCC -> 5V, GND -> GND, SCL -> A5, SDA -> A4
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
const uint8_t OLED_ADDRESS = 0x3C;

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire, -1);   // -1 = no reset pin

void setup() {
  Serial.begin(9600);
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("OLED not found. Check wiring and address."));
    while (true) delay(100);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 28);
  display.print(F("Warming up..."));
  display.display();
  delay(2000);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  display.clearDisplay();
  if (isnan(h) || isnan(t)) {
    display.setTextSize(1);
    display.setCursor(0, 28);
    display.print(F("Sensor error :("));
    display.display();
    delay(2000);
    return;
  }

  // Pick a kaomoji face for the mood
  const char* face = "(^_^)";
  if (t > 28)        face = "(>_<)";   // hot
  else if (t < 18)   face = "(o_o)";   // cold
  else if (h > 65)   face = "(~_~)";   // humid

  // Big temperature
  display.setTextSize(3);
  display.setCursor(0, 0);
  display.print(t, 1);
  display.setTextSize(1);
  display.print(F(" o"));               // small degree mark
  display.setTextSize(2);
  display.print(F("C"));

  // Humidity and face
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.print((int)h);
  display.print(F("%"));
  display.setCursor(64, 32);
  display.print(face);

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print(F("humidity     mood"));
  display.display();

  Serial.print(t);
  Serial.print(F(" C  "));
  Serial.print(h);
  Serial.println(F(" %"));
  delay(2500);                       // DHT22: max one reading every 2 seconds
}
