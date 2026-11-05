/*
  Fridge Door Alarm — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Light sensor module VCC -> 5V, GND -> GND, DO -> Pin 2
          Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
  Calibrate the module's blue knob so its little DO light is ON in light and OFF in the dark.
*/

const int LIGHT_DO = 2;
const int BUZZER = 8;
const unsigned long OPEN_LIMIT_MS = 20000;   // beep after 20 seconds of light

// On these modules DO goes LOW when it's bright (that's when the DO light is on).
// If your module behaves the other way, change this to false.
const bool BRIGHT_WHEN_LOW = true;

bool wasBright = false;
unsigned long brightSince = 0;

void setup() {
  pinMode(LIGHT_DO, INPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  Serial.println("Fridge door alarm ready");
}

void loop() {
  bool lowSignal = (digitalRead(LIGHT_DO) == LOW);
  bool bright = (lowSignal == BRIGHT_WHEN_LOW);

  if (bright && !wasBright) {
    brightSince = millis();
    Serial.println("BRIGHT (door open?)");
  }
  if (!bright && wasBright) {
    noTone(BUZZER);
    Serial.println("DARK (door closed)");
  }
  wasBright = bright;

  if (bright && millis() - brightSince > OPEN_LIMIT_MS) {
    tone(BUZZER, 2000, 150);   // beep...
    delay(300);                // ...pause, so it goes beep-beep-beep
  } else {
    delay(50);
  }
}
