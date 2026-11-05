/*
  Wi-Fi Radar — Parts Lab
  Boards: ESP32 DevKit / ESP32-C3 SuperMini / ESP32-S3 DevKitC (just the board!)
  Open the Serial Monitor at 115200 baud.
  (C3 SuperMini: set "USB CDC On Boot: Enabled" in the Tools menu.)
*/
#include <WiFi.h>

const int MAX_SHOWN = 30;

// Turn signal strength (dBm) into 1-4 bars
String bars(int rssi) {
  int n = rssi >= -55 ? 4 : rssi >= -67 ? 3 : rssi >= -78 ? 2 : 1;
  String s = "";
  for (int i = 0; i < 4; i++) s += (i < n) ? '#' : '.';
  return s;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);   // "station" mode: we listen, we don't create a network
  WiFi.disconnect();
  delay(100);
  Serial.println("Wi-Fi Radar ready");
}

void loop() {
  Serial.println();
  Serial.println("Scanning...");
  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("No networks found");
  } else {
    // Sort network numbers by signal strength (strongest first)
    int order[MAX_SHOWN];
    int count = min(n, MAX_SHOWN);
    for (int i = 0; i < count; i++) order[i] = i;
    for (int i = 0; i < count; i++) {
      for (int j = i + 1; j < count; j++) {
        if (WiFi.RSSI(order[j]) > WiFi.RSSI(order[i])) { int t = order[i]; order[i] = order[j]; order[j] = t; }
      }
    }
    Serial.printf("Found %d networks:\n", n);
    for (int k = 0; k < count; k++) {
      int i = order[k];
      String name = WiFi.SSID(i);
      if (name.length() == 0) name = "(hidden)";
      bool open = WiFi.encryptionType(i) == WIFI_AUTH_OPEN;
      Serial.printf("%2d. %-24s %s %4d dBm  ch %2d  %s\n", k + 1, name.c_str(), bars(WiFi.RSSI(i)).c_str(),
                    (int)WiFi.RSSI(i), (int)WiFi.channel(i), open ? "OPEN" : "locked");
    }
  }
  WiFi.scanDelete();   // free the memory used by the results
  delay(10000);
}
