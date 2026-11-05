/*
  Pico W Web Remote — Parts Lab
  Board: Raspberry Pi Pico W (just the board!)
  Open the IP address shown in the Serial Monitor (115200) on your phone.
*/
#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME

WebServer server(80);
bool ledOn = false;

String page() {
  String s = "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
  s += "<title>Pico W Remote</title><style>body{font-family:system-ui,sans-serif;background:#0b0e18;color:#e8f1ff;text-align:center;padding:24px}";
  s += "a{display:inline-block;font-size:22px;padding:18px 28px;margin:8px;border-radius:16px;background:#36f1cd;color:#04110e;text-decoration:none}";
  s += "a.off{background:#2a3150;color:#fff}</style></head><body><h2>Pico W Remote</h2>";
  s += "<p>LED is <b>" + String(ledOn ? "ON" : "OFF") + "</b></p>";
  s += "<p>Chip temperature: " + String(analogReadTemp(), 1) + " &deg;C</p>";
  s += "<a href='/on'>LED ON</a><a class='off' href='/off'>LED OFF</a></body></html>";
  return s;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nOpen http://");
  Serial.println(WiFi.localIP());

  server.on("/", []() { server.send(200, "text/html", page()); });
  server.on("/on", []() { ledOn = true; digitalWrite(LED_BUILTIN, HIGH); server.send(200, "text/html", page()); });
  server.on("/off", []() { ledOn = false; digitalWrite(LED_BUILTIN, LOW); server.send(200, "text/html", page()); });
  server.begin();
}

void loop() {
  server.handleClient();
}
