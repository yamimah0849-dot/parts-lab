/*
  Chiptune Music Box — Parts Lab
  Board: ESP32 DevKit
  Wiring: MAX98357A: VIN -> VIN (5V), GND -> GND, BCLK -> GPIO 26, LRC -> GPIO 25, DIN -> GPIO 22
          Speaker -> MAX98357A + and -   (never to GND)
          Optional next-song button: GPIO 4 <-> GND
  All melodies are original compositions.
*/
#include <ESP_I2S.h>

I2SClass i2s;
const int SAMPLE_RATE = 22050;
int VOLUME = 1500;          // square-wave height (max 32767). START LOW!
const int BUTTON = 4;

#define R 0
// Note frequencies (Hz)
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define B5 988
#define C6 1047
#define D6 1175
#define E6 1319
#define G4 392
#define A4 440
#define B4 494

struct Song { const char* name; const int (*notes)[2]; int count; };

const int labTheme[][2] = {{C5,120},{E5,120},{G5,120},{C6,360},{B5,120},{G5,120},{A5,240},{F5,240},{G5,120},{E5,120},{C5,120},{D5,120},{E5,480},{R,120},{G5,120},{C6,600}};
const int pixelQuest[][2] = {{E5,150},{E5,150},{R,150},{E5,150},{R,150},{C5,150},{E5,300},{G5,300},{R,300},{G4,300},{R,300},{C5,300},{G4,300},{E5,450},{A5,300},{B5,300},{A5,150},{G5,450}};
const int bossBattle[][2] = {{A4,120},{C5,120},{E5,120},{A5,120},{G5,120},{E5,120},{D5,120},{E5,120},{A4,120},{C5,120},{E5,120},{B5,240},{A5,120},{G5,120},{E5,240},{A5,480}};
const Song SONGS[] = {
  {"Lab Theme", labTheme, sizeof(labTheme) / sizeof(labTheme[0])},
  {"Pixel Quest", pixelQuest, sizeof(pixelQuest) / sizeof(pixelQuest[0])},
  {"Boss Battle", bossBattle, sizeof(bossBattle) / sizeof(bossBattle[0])},
};
const int NUM_SONGS = 3;
int song = 0;
bool skip = false;

// Play one square-wave note. The last 15% is silent so notes don't blur together.
void playNote(int freq, int ms) {
  int total = (long)SAMPLE_RATE * ms / 1000;
  int soundPart = total * 85 / 100;
  int16_t buf[128];
  float phase = 0;
  int i = 0;
  while (i < total) {
    int n = min(128, total - i);
    for (int k = 0; k < n; k++) {
      int16_t s = 0;
      if (freq > 0 && i + k < soundPart) s = (phase < 0.5f) ? VOLUME : -VOLUME;
      buf[k] = s;
      phase += (float)freq / SAMPLE_RATE;
      if (phase >= 1.0f) phase -= 1.0f;
    }
    i2s.write((uint8_t*)buf, n * sizeof(int16_t));
    i += n;
  }
  if (digitalRead(BUTTON) == LOW) skip = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  i2s.setPins(26, 25, 22);   // BCLK, LRC (WS), DIN
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("I2S start failed");
    while (true) delay(100);
  }
}

void loop() {
  Serial.print("Now playing: ");
  Serial.println(SONGS[song].name);
  skip = false;
  for (int rep = 0; rep < 2 && !skip; rep++) {
    for (int n = 0; n < SONGS[song].count && !skip; n++) playNote(SONGS[song].notes[n][0], SONGS[song].notes[n][1]);
  }
  while (digitalRead(BUTTON) == LOW) delay(5);
  playNote(R, 400);
  song = (song + 1) % NUM_SONGS;
}
