/*
  Pocket Jukebox — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Wiring: Passive buzzer (+) -> 220 ohm -> Pin 8, (-) -> GND
          Button: Pin 2 <-> GND   (or TTP223 touch: SIG -> Pin 2, VCC -> 5V, GND -> GND)
  Press to play the next song. Press during a song to skip it.
*/

const int BUZZER = 8;
const int BUTTON = 2;
const bool USE_TOUCH = false;   // set to true if you use a TTP223 touch pad

// Note frequencies in Hz
#define REST 0
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define B5 988
#define C6 1047

// Each song is a list of {note, length}. Length is in eighth notes (2 = quarter note).
const int labTheme[][2] = {   // an original jingle for Parts Lab
  {C5,1},{E5,1},{G5,1},{C6,3},{B5,1},{G5,1},{A5,2},{F5,2},
  {G5,1},{E5,1},{C5,1},{D5,1},{E5,4},{REST,1},{G5,1},{C6,6}
};
const int twinkle[][2] = {    // traditional (public domain)
  {C4,2},{C4,2},{G4,2},{G4,2},{A4,2},{A4,2},{G4,4},
  {F4,2},{F4,2},{E4,2},{E4,2},{D4,2},{D4,2},{C4,4}
};
const int odeToJoy[][2] = {   // Beethoven (public domain)
  {E4,2},{E4,2},{F4,2},{G4,2},{G4,2},{F4,2},{E4,2},{D4,2},
  {C4,2},{C4,2},{D4,2},{E4,2},{E4,3},{D4,1},{D4,4}
};
const int birthday[][2] = {   // public domain
  {G4,1},{G4,1},{A4,2},{G4,2},{C5,2},{B4,4},
  {G4,1},{G4,1},{A4,2},{G4,2},{D5,2},{C5,4},
  {G4,1},{G4,1},{G5,2},{E5,2},{C5,2},{B4,2},{A4,4},
  {F5,1},{F5,1},{E5,2},{C5,2},{D5,2},{C5,4}
};

const char* names[] = {"Lab Theme", "Twinkle Twinkle", "Ode to Joy", "Happy Birthday"};
int song = 0;

bool buttonDown() {
  return USE_TOUCH ? digitalRead(BUTTON) == HIGH : digitalRead(BUTTON) == LOW;
}

void waitRelease() {
  while (buttonDown()) delay(5);
  delay(30);   // debounce
}

// Play one song. Returns early (true) if the button is pressed.
bool play(const int notes[][2], int count, int eighthMs) {
  for (int i = 0; i < count; i++) {
    int freq = notes[i][0];
    int len = notes[i][1] * eighthMs;
    if (freq != REST) tone(BUZZER, freq, len * 9 / 10);   // small gap between notes
    unsigned long start = millis();
    while (millis() - start < (unsigned long)len) {
      if (buttonDown()) { noTone(BUZZER); return true; }
    }
  }
  return false;
}

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, USE_TOUCH ? INPUT : INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Pocket Jukebox ready. Press the button!");
}

void loop() {
  if (!buttonDown()) return;   // idle until pressed
  waitRelease();

  Serial.print("Now playing: ");
  Serial.println(names[song]);
  bool skipped = false;
  switch (song) {
    case 0: skipped = play(labTheme, sizeof(labTheme) / sizeof(labTheme[0]), 120); break;
    case 1: skipped = play(twinkle, sizeof(twinkle) / sizeof(twinkle[0]), 200); break;
    case 2: skipped = play(odeToJoy, sizeof(odeToJoy) / sizeof(odeToJoy[0]), 200); break;
    case 3: skipped = play(birthday, sizeof(birthday) / sizeof(birthday[0]), 180); break;
  }
  song = (song + 1) % 4;
  if (skipped) {
    // Wait for the skip press to end; the next press plays the next song
    waitRelease();
    Serial.println("Skipped!");
  }
}
