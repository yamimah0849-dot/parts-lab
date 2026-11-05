/*
  SD Weather Logger — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: DHT sensor library, Adafruit Unified Sensor, RTClib, SD
  Wiring: DHT22: + -> 5V, - -> GND, DATA -> Pin 2
          SD module: VCC -> 5V, GND -> GND, CS -> 10, MOSI -> 11, MISO -> 12, SCK -> 13
          DS3231: VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5
*/
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <DHT.h>

DHT dht(2, DHT22);
RTC_DS3231 rtc;
const int SD_CS = 10;
const char* FILE_NAME = "LOG.CSV";
int lastMinute = -1;

void setup() {
  Serial.begin(9600);
  dht.begin();
  if (!rtc.begin()) { Serial.println(F("DS3231 not found")); while (true) delay(100); }
  if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (!SD.begin(SD_CS)) { Serial.println(F("SD card failed: is it FAT32 and inserted?")); while (true) delay(100); }

  if (!SD.exists(FILE_NAME)) {               // first time: write the column names
    File f = SD.open(FILE_NAME, FILE_WRITE);
    if (f) { f.println(F("datetime,temp_c,humidity")); f.close(); }
  }
  Serial.println(F("Logging every minute..."));
}

void loop() {
  DateTime now = rtc.now();
  if (now.minute() != lastMinute) {
    lastMinute = now.minute();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(t) || isnan(h)) { Serial.println(F("DHT read failed")); return; }

    // Build a line like "2026-11-05 14:32,24.1,48.0"
    char tBuf[8], hBuf[8], line[40];
    dtostrf(t, 0, 1, tBuf);    // dtostrf turns a float into text on AVR boards
    dtostrf(h, 0, 1, hBuf);
    snprintf(line, sizeof(line), "%04d-%02d-%02d %02d:%02d,%s,%s",
             now.year(), now.month(), now.day(), now.hour(), now.minute(), tBuf, hBuf);

    File f = SD.open(FILE_NAME, FILE_WRITE);
    if (f) {
      f.println(line);
      f.close();               // closing saves the data to the card
      Serial.println(line);
    } else {
      Serial.println(F("Could not open the log file"));
    }
  }
  delay(1000);
}
