/*
  Room Climate Display — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: DHT sensor library, Adafruit Unified Sensor, LiquidCrystal I2C
  Wiring: DHT11 module: + -> 5V, - -> GND, DATA -> Pin 2
          LCD (I2C backpack): GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
*/
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11            // change to DHT22 if you use a DHT22
const uint8_t LCD_ADDRESS = 0x27; // try 0x3F if the screen stays blank

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Room Climate");
  lcd.setCursor(0, 1);
  lcd.print("Warming up...");
  delay(2000);                    // the DHT11 needs a moment after power-up
}

void loop() {
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();

  lcd.clear();
  if (isnan(humidity) || isnan(tempC)) {   // reading failed
    lcd.print("Sensor error!");
    lcd.setCursor(0, 1);
    lcd.print("Check wiring");
    Serial.println("Failed to read from DHT sensor");
    delay(2000);
    return;
  }

  // Line 1: "24.0C  45%"
  lcd.setCursor(0, 0);
  lcd.print(tempC, 1);
  lcd.print((char)223);            // the degree symbol on these LCDs
  lcd.print("C  ");
  lcd.print((int)humidity);
  lcd.print("% RH");

  // Line 2: a comfort message
  lcd.setCursor(0, 1);
  if (tempC > 28)          lcd.print("Too hot!");
  else if (tempC < 18)     lcd.print("Brr, chilly...");
  else if (humidity > 65)  lcd.print("Too humid!");
  else if (humidity < 30)  lcd.print("Very dry air");
  else                     lcd.print("Comfy :)");

  Serial.print(tempC);
  Serial.print(" C, ");
  Serial.print(humidity);
  Serial.println(" %");
  delay(2000);                     // DHT11: one reading every 1-2 seconds
}
