/*
  Clap Clap Lamp — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: KY-038 sound module: + -> 5V, G -> GND, D0 -> Pin 2
          LED (+) -> 220 ohm -> Pin 9, LED (-) -> GND
  Tune the module's screw so its D0 LED is off in silence and flashes on a clap.
*/

const int SOUND = 2;
const int LED = 9;
const unsigned long MIN_GAP = 150;    // claps closer than this are one clap (echo)
const unsigned long MAX_GAP = 700;    // second clap must come before this

bool lampOn = false;
unsigned long lastClap = 0;
int clapCount = 0;

void setup() {
  pinMode(SOUND, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Clap twice!");
}

void loop() {
  unsigned long now = millis();

  if (digitalRead(SOUND) == HIGH) {            // loud sound detected
    unsigned long gap = now - lastClap;
    if (gap > MIN_GAP) {
      if (clapCount == 1 && gap < MAX_GAP) {
        lampOn = !lampOn;
        digitalWrite(LED, lampOn ? HIGH : LOW);
        Serial.println(lampOn ? "Double clap! Lamp ON" : "Double clap! Lamp OFF");
        clapCount = 0;
      } else {
        clapCount = 1;                           // first clap
      }
      lastClap = now;
    }
  }

  if (clapCount == 1 && now - lastClap > MAX_GAP) clapCount = 0;   // too slow: reset
}
