/*
  Smart Door Lock 🔐

  Features:
  - Password unlock using Keypad 4x4
  - LCD1602 display
  - Servo motor lock control
  - Buzzer notification
  - EEPROM password storage
  - Password change function

  Default Password: 1234

  Controls:
  - Enter PIN + #
  - A = Change password
  - * = Clear input

  Hardware:
  - Arduino Uno R3
  - Keypad 4x4
  - LCD1602
  - SG90 Servo
  - Buzzer
*/

#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(A1, A2, A3, A4, A5, 11);

Servo lockServo;

const int buzzer = 10;   // Buzzer pin

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 2, A0};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

// Password variables
String password = "";
String input = "";

bool changeMode = false;

// Display home screen
void showHome() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART LOCK");

  lcd.setCursor(0, 1);
  lcd.print("PIN:");
}

// Load password from EEPROM
void loadPassword() {

  char pass[5];

  for (int i = 0; i < 4; i++) {
    pass[i] = EEPROM.read(i);
  }

  pass[4] = '\0';

  // EEPROM is empty when value = 255
  if (pass[0] == (char)255) {

    password = "1234";

    for (int i = 0; i < 4; i++) {
      EEPROM.write(i, password[i]);
    }

  } else {

    password = String(pass);
  }
}

// Save new password to EEPROM
void savePassword(String newPass) {

  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, newPass[i]);
  }

  password = newPass;
}

void setup() {

  lockServo.attach(3);
  lockServo.write(0);

  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);

  loadPassword();

  lcd.clear();
  lcd.print("SMART LOCK");

  delay(1500);

  showHome();
}

void loop() {

  char key = keypad.getKey();

  if (!key) {
    return;
  }

  // A = Change password
  if (key == 'A') {

    changeMode = true;
    input = "";

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("NEW PASSWORD");

    lcd.setCursor(0, 1);
    lcd.print("4 DIGITS + #");

    tone(buzzer, 1500, 200);

    return;
  }

  // * = Clear current input
  if (key == '*') {

    input = "";
    showHome();

    return;
  }

  // # = Confirm
  if (key == '#') {

    // Password change mode
    if (changeMode) {

      if (input.length() == 4) {

        savePassword(input);

        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("PASSWORD");

        lcd.setCursor(0, 1);
        lcd.print("CHANGED");

        tone(buzzer, 1800, 300);

      } else {

        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("PIN MUST BE");

        lcd.setCursor(0, 1);
        lcd.print("4 DIGITS");
      }

      delay(2000);

      changeMode = false;
      input = "";

      showHome();

      return;
    }

    // Verify password
    if (input == password) {

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("ACCESS");

      lcd.setCursor(0, 1);
      lcd.print("GRANTED");

      tone(buzzer, 1200, 200);

      lockServo.write(90);

      delay(3000);

      lockServo.write(0);

    } else {

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("ACCESS");

      lcd.setCursor(0, 1);
      lcd.print("DENIED");

      for (int i = 0; i < 3; i++) {

        tone(buzzer, 500, 200);
        delay(300);
      }
    }

    input = "";

    delay(1500);

    showHome();

    return;
  }

  // Enter PIN
  if (input.length() < 4) {

    input += key;

    lcd.setCursor(4, 1);

    for (int i = 0; i < input.length(); i++) {
      lcd.print("*");
    }
  }
}
