#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int STEPS_PER_REV = 200;
const int dirPin = 2;
const int stepPin = 3;
int stepsRequired = 65;
int mSpeed = 2000;
int lockStatus = 0; // 0 is unlocked, 1 is locked
const byte ROWS = 4;
const byte COLS = 4;
char keymap[ROWS][COLS] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, 7, 6};
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, ROWS, COLS);
String correctCode = "AB123" ;
char clearKey = 'C';
String codeEntered = "00000";
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
int relayPin = 4;
int statusReadButton = 5;
int buttonStatus = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(relayPin, OUTPUT);
  pinMode(statusReadButton, INPUT);
}

void loop()
{
  char keypressed = myKeypad.getKey(); // wait for a key to be pressed
  if (keypressed)
  {
    codeEntered[numberOfKeys] = keypressed;
    numberOfKeys = numberOfKeys + 1;
    Serial.println(keypressed);
    // print code to screen
    if (codeEntered == correctCode) // correct code
    {
      numberOfKeys = 0;
      codeEntered = "00000";
      if (lockStatus == 0) // lock door if open
      {
        lockStatus = 1;
        delay (2000);
        // print to screen
      }
      else if (lockStatus == 1) // open door if locked
      {
        lockStatus = 0;
        delay (2000);
        //print to screen
      }
    }
    else if (keypressed == clearKey) // clear key
    {
      delay(2000);
      //print to screen
      codeEntered = "00000";
      numberOfKeys = 0;
    }
    else if (numberOfKeys >= 5) // incorrect code
    {
      codeEntered = "00000";
      numberOfKeys = 0;
      // print to screen 
      delay(2000);
    }
  }
  buttonStatus = digitalRead(statusReadButton);
  if (buttonStatus == 1 && numberOfKeys == 0) // open/lock if button is pressed and no keys are entered on keypad
  {
    if (lockStatus == 0)
    {
      delay(500);
      lockStatus = 1;
      numberOfKeys = 0;
    }
    else if (lockStatus == 1)
    {
      delay(2000);
      lockStatus = 0;
      numberOfKeys = 0;
    }
  }
}
