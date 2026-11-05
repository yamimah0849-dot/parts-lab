/*
  Wi-Fi Mood Ring — Parts Lab
  Board: ESP32 DevKit
  Library: Adafruit NeoPixel
  Wiring: WS2812B 16-LED ring: 5V -> VIN, GND -> GND, DIN -> 330 ohm -> GPIO 16
  Open http://partslab-ring.local (or the IP from the Serial Monitor at 115200).
*/
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_NeoPixel.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME

const int RING_PIN = 16;
const int NUM_LEDS = 16;
const uint8_t MAX_BRIGHTNESS = 64;   // ~25%: keeps 16 LEDs safe on USB power

Adafruit_NeoPixel ring(NUM_LEDS, RING_PIN, NEO_GRB + NEO_KHZ800);
WebServer server(80);
bool rainbow = false;
uint16_t firstHue = 0;

const char PAGE[] = R"HTML(<!doctype html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Mood Ring</title>
<style>body{font-family:system-ui,sans-serif;background:#0b0e18;color:#e8f1ff;text-align:center;padding:24px}
input[type=color]{width:180px;height:180px;border:0;border-radius:50%;background:none}
button{font-size:20px;padding:16px 26px;margin:8px;border-radius:16px;border:0;background:#ff4fa3;color:#fff}
button.off{background:#2a3150}</style></head>
<body><h2>&#128141; Mood Ring</h2><input type="color" value="#36f1cd" oninput="fetch('/color?hex='+this.value.slice(1))">
<p><button onclick="fetch('/rainbow')">&#127752; Rainbow</button><button class="off" onclick="fetch('/off')">Off</button></p>
</body></html>)HTML";

void fill(uint32_t c) {
  for (int i = 0; i < NUM_LEDS; i++) ring.setPixelColor(i, c);
  ring.show();
}

void setup() {
  Serial.begin(115200);
  ring.begin();
  ring.setBrightness(MAX_BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {       // start-up color wipe
    ring.setPixelColor(i, ring.Color(54, 241, 205));
    ring.show();
    delay(40);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nOpen http://");
  Serial.println(WiFi.localIP());
  MDNS.begin("partslab-ring");

  server.on("/", []() { server.send(200, "text/html", String(PAGE)); });
  server.on("/color", []() {
    long rgb = strtol(server.arg("hex").c_str(), nullptr, 16);   // "ff8800" -> number
    rainbow = false;
    fill(ring.Color((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF));
    server.send(200, "text/plain", "ok");
  });
  server.on("/rainbow", []() { rainbow = true; server.send(200, "text/plain", "ok"); });
  server.on("/off", []() { rainbow = false; fill(0); server.send(200, "text/plain", "ok"); });
  server.begin();
}

void loop() {
  server.handleClient();
  static unsigned long last = 0;
  if (rainbow && millis() - last > 20) {
    last = millis();
    ring.rainbow(firstHue);       // spread the whole color wheel around the ring
    ring.show();
    firstHue += 256;
  }
}
