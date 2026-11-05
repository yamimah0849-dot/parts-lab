/*
  Morse Code Messenger — Parts Lab
  Boards: Arduino Uno / Nano / Mega / Leonardo
  Needs only the board (uses the built-in "L" LED).
  Optional passive buzzer: (+) -> 220 ohm -> Pin 8, (-) -> GND
  Open the Serial Monitor at 9600 baud, type a message and press Enter.
*/

const int BUZZER = 8;
const int UNIT = 120;          // dot length in milliseconds
const int TONE_HZ = 700;

// Morse code for A..Z then 0..9
const char* LETTERS[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
  "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};
const char* NUMBERS[10] = {
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
};

void signal(int units) {
  digitalWrite(LED_BUILTIN, HIGH);
  tone(BUZZER, TONE_HZ);
  delay(UNIT * units);
  digitalWrite(LED_BUILTIN, LOW);
  noTone(BUZZER);
  delay(UNIT);                 // 1-unit gap inside a letter
}

void sendChar(char c) {
  const char* code = nullptr;
  if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';   // make uppercase
  if (c >= 'A' && c <= 'Z') code = LETTERS[c - 'A'];
  else if (c >= '0' && c <= '9') code = NUMBERS[c - '0'];
  else if (c == ' ') { Serial.print(" / "); delay(UNIT * 4); return; }   // word gap
  if (code == nullptr) return;                   // skip unknown symbols

  Serial.print(c);
  Serial.print(' ');
  Serial.print(code);
  Serial.print("  ");
  for (int i = 0; code[i] != '\0'; i++) {
    signal(code[i] == '.' ? 1 : 3);
  }
  delay(UNIT * 2);             // letter gap = 3 units total
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  Serial.println("Type a message and press Enter:");
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    message.trim();
    if (message.length() == 0) return;
    Serial.print("Sending: ");
    Serial.println(message);
    for (unsigned int i = 0; i < message.length(); i++) sendChar(message[i]);
    Serial.println();
    Serial.println("Done! Type another message:");
  }
}
