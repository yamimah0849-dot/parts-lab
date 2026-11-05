/*
  Motion Alert to your iPhone — Parts Lab
  Board: ESP32 DevKit
  Wiring: PIR HC-SR501: VCC -> VIN (5V), OUT -> GPIO 27, GND -> GND
  Phone: install the free "ntfy" app and subscribe to the same topic as NTFY_TOPIC.
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME
const char* NTFY_TOPIC = "partslab-room-CHANGE-ME-7k3q9x";   // CHANGE ME: long and secret!

const int PIR_PIN = 27;
const unsigned long COOLDOWN_MS = 30000;   // at most one alert every 30 s
unsigned long lastAlert = 0;

bool sendAlert(const String& message) {
  WiFiClientSecure client;
  client.setInsecure();   // skips HTTPS certificate checks: OK for a hobby alert, never for secrets
  HTTPClient http;
  String url = String("https://ntfy.sh/") + NTFY_TOPIC;
  if (!http.begin(client, url)) return false;
  http.addHeader("Title", "Parts Lab alert");
  http.addHeader("Tags", "rotating_light");   // shows a siren emoji on the phone
  int code = http.POST(message);
  http.end();
  Serial.printf("ntfy answered: %d\n", code);
  return code == 200;
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println(" connected!");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  WiFi.mode(WIFI_STA);
  connectWiFi();
  Serial.println("Give the PIR about 1 minute to settle...");
}

void loop() {
  connectWiFi();   // reconnect if the Wi-Fi dropped
  if (digitalRead(PIR_PIN) == HIGH && (lastAlert == 0 || millis() - lastAlert > COOLDOWN_MS)) {
    Serial.println("Motion! Sending alert...");
    if (sendAlert("Motion detected in the lab")) Serial.println("Alert sent.");
    lastAlert = millis();
  }
  delay(100);
}
