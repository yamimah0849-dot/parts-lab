/*
  Phone-Controlled Lamp (Wi-Fi) — Parts Lab
  Board: ESP32 DevKit
  Wiring: LED (+) -> 100 ohm -> GPIO 16, LED (-) -> GND
  Open http://partslab-lamp.local (or the IP shown in the Serial Monitor at 115200) on your phone.
*/
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME

const int LED_PIN = 16;
WebServer server(80);
bool lampOn = false;
int level = 255;   // brightness 1..255

// The web page (stored as one big text block)
const char PAGE[] = R"HTML(<!doctype html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Lab Lamp</title>
<style>body{font-family:system-ui,sans-serif;background:#0b0e18;color:#e8f1ff;text-align:center;padding:24px}
h1{color:#36f1cd}button{font-size:22px;padding:18px 30px;margin:8px;border-radius:16px;border:0;background:#36f1cd;color:#04110e}
button.off{background:#2a3150;color:#fff}input{width:85%;height:44px}#s{font-size:18px;color:#ffb547}</style></head>
<body><h1>&#128161; Lab Lamp</h1><p id="s">...</p>
<button onclick="go('/on')">ON</button><button class="off" onclick="go('/off')">OFF</button>
<p>Brightness</p><input type="range" min="1" max="255" value="255" oninput="go('/level?v='+this.value)">
<script>function go(u){fetch(u).then(r=>r.text()).then(t=>document.getElementById('s').textContent=t)}go('/state')</script>
</body></html>)HTML";

void applyLamp() { analogWrite(LED_PIN, lampOn ? level : 0); }
String stateText() { return lampOn ? "Lamp is ON (" + String(level * 100 / 255) + "%)" : "Lamp is OFF"; }
void sendState() { server.send(200, "text/plain", stateText()); }

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - t0 > 20000) {
      Serial.println("\nStill not connected: check the name/password (2.4 GHz only).");
      t0 = millis();
    }
  }
  Serial.print("\nConnected! Open http://");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("partslab-lamp")) Serial.println("...or http://partslab-lamp.local");

  server.on("/", []() { server.send(200, "text/html", String(PAGE)); });
  server.on("/on", []() { lampOn = true; applyLamp(); sendState(); });
  server.on("/off", []() { lampOn = false; applyLamp(); sendState(); });
  server.on("/level", []() {
    level = constrain(server.arg("v").toInt(), 1, 255);
    lampOn = true;
    applyLamp();
    sendState();
  });
  server.on("/state", sendState);
  server.begin();
}

void loop() {
  server.handleClient();   // answer requests from the phone
}
