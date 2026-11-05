/*
  Rainbow Status Light — Parts Lab
  Board: ESP32-S3 DevKitC-1 (just the board!)
  Uses the built-in RGB LED. Type red, green, blue, pink, white, off or rainbow in the Serial Monitor (115200, Newline).
*/

const int RGB_PIN = 48;          // most DevKitC-1 boards. Some versions use 38.
const int BRIGHT = 40;           // the onboard LED is very bright, so keep it gentle

bool rainbowMode = true;
int hue = 0;

void setRGB(int r, int g, int b) {
  rgbLedWrite(RGB_PIN, r * BRIGHT / 255, g * BRIGHT / 255, b * BRIGHT / 255);
}

void setHue(int h) {             // h = 0..359
  int x = (h % 60) * 255 / 60;
  switch (h / 60) {
    case 0:  setRGB(255, x, 0); break;
    case 1:  setRGB(255 - x, 255, 0); break;
    case 2:  setRGB(0, 255, x); break;
    case 3:  setRGB(0, 255 - x, 255); break;
    case 4:  setRGB(x, 0, 255); break;
    default: setRGB(255, 0, 255 - x);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Type: red, green, blue, pink, white, off, rainbow");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
    rainbowMode = false;
    if (cmd == "red") setRGB(255, 0, 0);
    else if (cmd == "green") setRGB(0, 255, 0);
    else if (cmd == "blue") setRGB(0, 0, 255);
    else if (cmd == "pink") setRGB(255, 40, 120);
    else if (cmd == "white") setRGB(255, 255, 255);
    else if (cmd == "off") setRGB(0, 0, 0);
    else { rainbowMode = true; }
    Serial.print("OK: ");
    Serial.println(rainbowMode ? "rainbow" : cmd);
  }
  if (rainbowMode) {
    setHue(hue);
    hue = (hue + 1) % 360;
    delay(15);
  }
}
