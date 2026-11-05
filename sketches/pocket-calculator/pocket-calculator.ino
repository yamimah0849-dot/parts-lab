/*
  Pocket Calculator — Parts Lab
  Boards: Arduino Uno / Nano
  Libraries: Keypad, LiquidCrystal I2C
  Wiring: Keypad rows (pins 1-4) -> 9, 8, 7, 6 ; columns (pins 5-8) -> 5, 4, 3, 2
          LCD (I2C): GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
  Keys: A = +   B = -   C = x   D = /   # = equals   * = clear
*/
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[4] = {9, 8, 7, 6};
byte colPins[4] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // try 0x3F if blank

String typed = "";      // what's shown on line 1
String current = "";    // the number being typed
double total = 0;
char pendingOp = 0;     // + - x / or 0 for none
bool error = false;

char opSymbol(char k) {
  switch (k) { case 'A': return '+'; case 'B': return '-'; case 'C': return 'x'; case 'D': return '/'; }
  return 0;
}

// Apply the waiting operator to the running total
void applyPending() {
  double value = current.toDouble();
  if (pendingOp == 0) total = value;
  else if (pendingOp == '+') total += value;
  else if (pendingOp == '-') total -= value;
  else if (pendingOp == 'x') total *= value;
  else if (pendingOp == '/') {
    if (value == 0) { error = true; return; }
    total /= value;
  }
  current = "";
}

void showResult() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  if (error) { lcd.print("Error"); return; }
  if (total == (long)total && fabs(total) < 1e9) lcd.print((long)total);   // whole number
  else lcd.print(total, 4);
}

void clearAll() {
  typed = ""; current = ""; total = 0; pendingOp = 0; error = false;
  lcd.clear();
  lcd.print("0");
}

void setup() {
  lcd.init();
  lcd.backlight();
  clearAll();
}

void loop() {
  char k = keypad.getKey();
  if (!k) return;

  if (k == '*') { clearAll(); return; }
  if (error) clearAll();

  if (k >= '0' && k <= '9') {
    if (current.length() < 9) { current += k; typed += k; }
  } else if (opSymbol(k)) {
    if (current.length() > 0) applyPending();
    pendingOp = opSymbol(k);
    typed += pendingOp;
  } else if (k == '#') {
    if (current.length() > 0) applyPending();
    pendingOp = 0;
    showResult();
    current = "";
    typed = "";
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(typed.length() > 16 ? typed.substring(typed.length() - 16) : typed);
}
