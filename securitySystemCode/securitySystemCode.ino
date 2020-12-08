#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//-----------------------STEPPER MOTOR CODE ----------------------------
const int STEPS_PER_REV = 200;
const int dirPin = 2;
const int stepPin = 3;
int stepsRequired = 65;
int mSpeed = 2000;
int lockStatus = 0; // 0 is unlocked, 1 is locked
//----------------------------------------------------------------------

//----------------------KEYPAD CODE-------------------------------------
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
int numberOfKeys = 0;
//-----------------------------------------------------------------------

//---------------------------LCD SCREEN CODE-----------------------------
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//-----------------------------------------------------------------------

//--------------------------RELAY CODE-----------------------------------
int relayPin = 4;
//-----------------------------------------------------------------------

//--------------------------PUSH BUTTON CODE-----------------------------
int statusReadButton = 5;
int buttonStatus = 0;
//-----------------------------------------------------------------------

void setup()
{
  Serial.begin(9600);
  //----------------------SETUP STEPPER----------------------------------
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  //---------------------------------------------------------------------

  //----------------------SETUP LCD SCREEN-------------------------------
  lcd.begin(16, 2);
  lcd.backlight();
  screenWrite("Enter Code      ", "_____           ");
  //---------------------------------------------------------------------

  //----------------------RELAY SETUP------------------------------------
  pinMode(relayPin, OUTPUT);
  relayPosition(0);
  //---------------------------------------------------------------------

  //---------------------PUSH BUTTON SETUP-------------------------------
  pinMode(statusReadButton, INPUT);
  //---------------------------------------------------------------------
}

void loop()
{
  char keypressed = myKeypad.getKey();
  if (keypressed)
  {
    relayPosition(1);
    codeEntered[numberOfKeys] = keypressed;
    numberOfKeys = numberOfKeys + 1;
    Serial.println(keypressed);
    writeCode(numberOfKeys - 1, keypressed);
    if (codeEntered == correctCode)
    {
      numberOfKeys = 0;
      codeEntered = "00000";
      if (lockStatus == 0)
      {
        Serial.println("Correct Code. Locking Door...");
        screenWrite("Correct Code.    ", "Locking Door... ");
        stepperTurn(stepsRequired, 1, mSpeed);
        Serial.println("Door Locked.    ");
        screenWrite("      Door      ", "    Locked!     ");
        lockStatus = 1;
        delay (2000);
        relayPosition(0);
        screenWrite("Enter Code      ", "_____           ");
      }
      else if (lockStatus == 1)
      {
        Serial.println("Correct Code. Unlocking Door...");
        screenWrite("Correct Code.    ", "Unlocking Door..");
        stepperTurn(stepsRequired, 0, mSpeed); // Rev , CCW , low speed
        Serial.println("Door Unlocked.  ");
        screenWrite("      Door      ", "    Unlocked!   ");
        lockStatus = 0;
        delay (2000);
        relayPosition(0);
        screenWrite("Enter Code      ", "_____           ");
      }
    }
    else if (keypressed == clearKey)
    {
      Serial.println("Code cleared.");
      screenWrite("      Code      ", "    Cleared     ");
      delay(2000);
      relayPosition(0);
      screenWrite("Enter Code      ", "_____           ");
      codeEntered = "00000";
      numberOfKeys = 0;
    }
    else if (numberOfKeys >= 5)
    {
      codeEntered = "00000";
      numberOfKeys = 0;
      Serial.println("Incorrect code. Try again...");
      screenWrite("Incorrect code. ", "  Try again...  ");
      delay(2000);
      relayPosition(0);
      screenWrite("Enter Code      ", "_____           ");
    }
  }
  buttonStatus = digitalRead(statusReadButton);
  Serial.println(buttonStatus);
  if (buttonStatus == 1 && numberOfKeys == 0)
  {
    if (lockStatus == 0)
    {
      screenWrite("    Locking     ", "      Door      ");
      relayPosition(1);
      delay(500);
      stepperTurn(stepsRequired, 1, mSpeed);
      delay(2000);
      relayPosition(0);
      screenWrite("Enter Code      ", "_____           ");
      lockStatus = 1;
      numberOfKeys = 0;
    }
    else if (lockStatus == 1)
    {
      screenWrite("   Unlocking    ", "      Door      ");
      relayPosition(1);
      delay(500);
      stepperTurn(stepsRequired, 0, mSpeed);
      delay(2000);
      relayPosition(0);
      screenWrite("Enter Code      ", "_____           ");
      lockStatus = 0;
      numberOfKeys = 0;
    }
  }
}


void screenWrite(char topWord[16], char bottomWord[16])
{
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(topWord);
  lcd.setCursor(0, 1);
  lcd.print(bottomWord);
}

void writeCode(int cursorPosition, char key)
{
  lcd.backlight();
  lcd.setCursor(cursorPosition, 1);
  lcd.print(key);
}

void stepperTurn(int revAmount, int motorDirection, int motorSpeed)
{
  digitalWrite(dirPin, motorDirection); // 1 for CW and 0 for CCW

  for (int x = 0; x < revAmount; x++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(motorSpeed); // Increase delay for slower rotation
    digitalWrite(stepPin, LOW);
    delayMicroseconds(motorSpeed);
  }
}

void relayPosition(int onOff)
{
  if (onOff == 1)
  {
    digitalWrite(relayPin, HIGH);
  }
  else
  {
    digitalWrite(relayPin, LOW);
  }
}
