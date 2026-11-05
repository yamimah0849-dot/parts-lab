/*
  Anime Sound Board — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Library: DFRobotDFPlayerMini
  Wiring: DFPlayer: VCC -> 5V, GND -> GND, TX -> Pin 10, Pin 11 -> 1k ohm -> RX
          Speaker -> SPK_1 and SPK_2
          Buttons -> Pins 2, 3, 4 (other legs to GND)
          Optional volume pot: middle -> A0, outer legs -> 5V and GND
  SD card: folder "mp3" with 0001.mp3, 0002.mp3, 0003.mp3
*/
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial playerSerial(10, 11);   // RX, TX
DFRobotDFPlayerMini player;
const int BUTTONS[3] = {2, 3, 4};
const bool USE_VOLUME_POT = false;     // set true if you wired the pot to A0
int volume = 20;                       // 0..30

void setup() {
  for (int i = 0; i < 3; i++) pinMode(BUTTONS[i], INPUT_PULLUP);
  Serial.begin(9600);
  playerSerial.begin(9600);
  if (!player.begin(playerSerial)) {
    Serial.println("DFPlayer not found: check the SD card and the TX/RX wires");
    while (true) delay(100);
  }
  Serial.println("DFPlayer ready");
  player.volume(volume);
}

void loop() {
  for (int i = 0; i < 3; i++) {
    if (digitalRead(BUTTONS[i]) == LOW) {
      Serial.print("Playing track ");
      Serial.println(i + 1);
      player.playMp3Folder(i + 1);    // plays /mp3/000X.mp3
      while (digitalRead(BUTTONS[i]) == LOW) delay(5);
      delay(40);
    }
  }
  if (USE_VOLUME_POT) {
    int v = map(analogRead(A0), 0, 1023, 0, 30);
    if (abs(v - volume) > 1) { volume = v; player.volume(volume); }
  }
}
