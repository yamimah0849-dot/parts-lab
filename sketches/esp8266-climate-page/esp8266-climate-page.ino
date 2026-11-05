/*
  ESP8266 Climate Web Page — Parts Lab
  Boards: NodeMCU ESP8266 / Wemos D1 mini
  Library: Adafruit AHTX0
  Wiring: AHT20: VCC -> 3V3, GND -> GND, SDA -> D2 (GPIO 4), SCL -> D1 (GPIO 5)
  Open http://partslab-air.local (or the IP from the Serial Monitor at 115200).
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME

Adafruit_AHTX0 aht;
ESP8266WebServer server(80);
float tempC = NAN, humidity = NAN;
unsigned long lastRead = 0;

const char PAGE[] PROGMEM = R"HTML(<!doctype html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Lab Air</title>
<style>body{font-family:system-ui,sans-serif;background:#0b0e18;color:#e8f1ff;text-align:center;padding:24px}
.big{font-size:64px;font-weight:800;color:#36f1cd;margin:10px}.h{color:#8b6cff}small{color:#8d9abb}</style></head>
<body><h2>&#127777; Lab Air</h2><div class="big" id="t">--</div><div class="big h" id="h">--</div><small id="u">...</small>
<script>function load(){fetch('/data').then(r=>r.json()).then(d=>{
document.getElementById('t').textContent=d.t===null?'error':d.t.toFixed(1)+' °C';
document.getElementById('h').textContent=d.h===null?'':Math.round(d.h)+' %';
document.getElementById('u').textContent='updated '+new Date().toLocaleTimeString();}).catch(()=>{});}
load();setInterval(load,3000);</script></body></html>)HTML";

void handleData() {
  String json = "{\"t\":" + (isnan(tempC) ? String("null") : String(tempC, 1)) +
                ",\"h\":" + (isnan(humidity) ? String("null") : String(humidity, 1)) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();                       // D2 = SDA, D1 = SCL on these boards
  if (!aht.begin()) Serial.println("AHT20 not found: check wiring");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nOpen http://");
  Serial.println(WiFi.localIP());
  MDNS.begin("partslab-air");

  server.on("/", []() { server.send_P(200, "text/html", PAGE); });
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  MDNS.update();
  if (millis() - lastRead > 2000) {
    lastRead = millis();
    sensors_event_t hum, temp;
    if (aht.getEvent(&hum, &temp)) {
      tempC = temp.temperature;
      humidity = hum.relative_humidity;
      Serial.printf("%.1f C  %.0f %%\n", tempC, humidity);
    }
  }
}
