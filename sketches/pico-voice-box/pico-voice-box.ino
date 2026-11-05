/*
  Pico Voice Line Box — Parts Lab
  Boards: Raspberry Pi Pico / Pico W / Pico 2 / Pico 2 W
  Library: DFRobotDFPlayerMini
  Wiring: DFPlayer: VCC -> VBUS (5V), GND -> GND, RX -> GP0 (Pico TX), TX -> GP1 (Pico RX)
          Speaker -> SPK_1 and SPK_2
          Buttons -> GP13, GP14, GP15 (other legs to GND)
  SD card: folder "mp3" with 0001.mp3, 0002.mp3, 0003.mp3
*/
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini player;
const int BUTTONS[3] = {13, 14, 15};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) pinMode(BUTTONS[i], INPUT_PULLUP);
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(9600);
  delay(1000);                       // the DFPlayer needs a moment after power-up
  if (!player.begin(Serial1)) {
    Serial.println("DFPlayer not found: check the SD card and the GP0/GP1 wires");
    while (true) delay(100);
  }
  player.volume(22);                 // 0..30
  Serial.println("DFPlayer ready");
}

void loop() {
  for (int i = 0; i < 3; i++) {
    if (digitalRead(BUTTONS[i]) == LOW) {
      Serial.print("Voice line ");
      Serial.println(i + 1);
      player.playMp3Folder(i + 1);   // /mp3/000X.mp3
      while (digitalRead(BUTTONS[i]) == LOW) delay(5);
      delay(40);
    }
  }
}
