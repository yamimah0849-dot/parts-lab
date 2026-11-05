/*
  Click-Click Lamp — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Button: Pin 2 <-> GND (built-in pull-up, no resistor needed)
          LED (+) -> 220 ohm -> Pin 9, LED (-) -> GND
  Tap = toggle on/off. Hold for 1 second = "breathing" mode.
*/

const int BUTTON_PIN = 2;
const int LED_PIN = 9;               // a PWM pin, so it can fade
const unsigned long DEBOUNCE_MS = 30; // ignore bounces shorter than this
const unsigned long HOLD_MS = 1000;   // how long counts as "hold"

bool lampOn = false;
bool breathing = false;

bool lastReading = HIGH;   // raw reading last time
bool stableState = HIGH;   // debounced state (HIGH = not pressed)
unsigned long lastChange = 0;
unsigned long pressStart = 0;
bool holdHandled = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  // Debounce: only accept a change if it stayed the same for DEBOUNCE_MS
  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }
  if (millis() - lastChange > DEBOUNCE_MS && reading != stableState) {
    stableState = reading;
    if (stableState == LOW) {            // button just pressed
      pressStart = millis();
      holdHandled = false;
    } else if (!holdHandled) {           // released after a SHORT press
      if (breathing) {
        breathing = false;
        lampOn = false;
      } else {
        lampOn = !lampOn;
      }
      Serial.println(lampOn ? "Lamp ON" : "Lamp OFF");
    }
  }

  // Long press detected while the button is still held
  if (stableState == LOW && !holdHandled && millis() - pressStart > HOLD_MS) {
    holdHandled = true;
    breathing = true;
    Serial.println("Breathing mode");
  }

  if (breathing) {
    // Smooth fade using a sine wave: one breath every 4 seconds
    float phase = (millis() % 4000) / 4000.0 * 2.0 * PI;
    int level = (int)((sin(phase - PI / 2.0) + 1.0) * 127.5);   // 0..255
    analogWrite(LED_PIN, level);
  } else {
    digitalWrite(LED_PIN, lampOn ? HIGH : LOW);
  }
}
