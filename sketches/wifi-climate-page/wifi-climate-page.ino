/*
  Wi-Fi Thermometer Page — Parts Lab
  Board: ESP32 DevKit
  Libraries: DHT sensor library, Adafruit Unified Sensor
  Wiring: DHT22: + -> 3V3, - -> GND, DATA -> GPIO 4
  Open http://partslab-climate.local (or the IP from the Serial Monitor at 115200).
*/
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME

DHT dht(4, DHT22);
WebServer server(80);
float tempC = NAN, humidity = NAN;
unsigned long lastRead = 0;

const char PAGE[] = R"HTML(<!doctype html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Lab Climate</title>
<style>body{font-family:system-ui,sans-serif;background:#0b0e18;color:#e8f1ff;text-align:center;padding:24px}
.big{font-size:64px;font-weight:800;color:#36f1cd;margin:10px}.h{color:#8b6cff}small{color:#8d9abb}</style></head>
<body><h2>&#127777; Lab Climate</h2><div class="big" id="t">--</div><div class="big h" id="h">--</div>
<small id="u">connecting...</small>
<script>
function load(){fetch('/data').then(r=>r.json()).then(d=>{
 document.getElementById('t').textContent=d.t===null?'error':d.t.toFixed(1)+' °C';
 document.getElementById('h').textContent=d.h===null?'':Math.round(d.h)+' %';
 document.getElementById('u').textContent='updated '+new Date().toLocaleTimeString();}).catch(()=>{});}
load();setInterval(load,3000);
</script></body></html>)HTML";

void handleData() {
  String json = "{\"t\":";
  json += isnan(tempC) ? "null" : String(tempC, 1);
  json += ",\"h\":";
  json += isnan(humidity) ? "null" : String(humidity, 1);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nOpen http://");
  Serial.println(WiFi.localIP());
  MDNS.begin("partslab-climate");

  server.on("/", []() { server.send(200, "text/html", String(PAGE)); });
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - lastRead > 2500) {      // the DHT22 needs 2 s between readings
    lastRead = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) { tempC = t; humidity = h; }
    Serial.printf("%.1f C  %.0f %%\n", tempC, humidity);
  }
}
