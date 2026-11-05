/*
  Plant Calls Your Phone — Parts Lab
  Board: ESP32 DevKit
  Wiring: Capacitive soil sensor v1.2: VCC -> 3V3, GND -> GND, AOUT -> GPIO 34
  Phone: install the free "ntfy" app and subscribe to NTFY_TOPIC.
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";          // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";  // CHANGE ME
const char* NTFY_TOPIC = "partslab-plant-CHANGE-ME-4m8x2";   // CHANGE ME: long and secret

const int SENSOR = 34;
int DRY_VALUE = 3000;    // reading in air (0..4095 on the ESP32)    CHANGE ME after calibrating
int WET_VALUE = 1300;    // reading in water                          CHANGE ME
const int THIRSTY_PCT = 30;
const unsigned long REMIND_MS = 6UL * 60 * 60 * 1000;   // remind every 6 hours

unsigned long lastAlert = 0;
bool alerted = false;

bool notify(const String& msg) {
  WiFiClientSecure client;
  client.setInsecure();   // skips certificate checks: OK for a hobby alert, never for secrets
  HTTPClient http;
  if (!http.begin(client, String("https://ntfy.sh/") + NTFY_TOPIC)) return false;
  http.addHeader("Title", "Parts Lab plant");
  http.addHeader("Tags", "seedling");
  int code = http.POST(msg);
  http.end();
  return code == 200;
}

int moisturePct() {
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(SENSOR); delay(10); }
  int pct = map(sum / 10, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(pct, 0, 100);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println(" connected!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
  int pct = moisturePct();
  Serial.printf("raw %d  moisture %d %%\n", analogRead(SENSOR), pct);

  if (pct < THIRSTY_PCT) {
    if (!alerted || millis() - lastAlert > REMIND_MS) {
      if (notify("Your plant is thirsty (" + String(pct) + "%). Please water me!")) {
        Serial.println("Alert sent");
        alerted = true;
        lastAlert = millis();
      }
    }
  } else if (pct > THIRSTY_PCT + 15) {
    alerted = false;   // watered: ready to alert again next time
  }
  delay(60000);        // check once a minute
}
