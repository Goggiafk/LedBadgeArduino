#include <Uduino.h>
#include<Uduino_Wifi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
//#include <Adafruit_TFTLCD.h>
#include <Fonts/FontsFree_Net_uni05_534pt7b.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 12

Uduino_Wifi uduino("uduinoBoard");


//char * dupaText="Staly Tekst";
#define TABLEN 100
char dupaText[TABLEN];
char staticText[TABLEN];

char animText1[TABLEN];
char animText2[TABLEN];
char animText3[TABLEN];

bool shouldBeStatic = false;
bool isInverse = false;
bool isPartyMode = false;
// textEffect_t scrollState = PA_SCROLL_LEFT;
// textPosition_t state = PA_CENTER;
int stateVal = 0;

int scrollSpeed = 100;
int partySpeed = 500;
bool isAnim = false;
char defChar = '`';

//Connect TX pin of the HC-05 to RX pin of the Arduino
//Connect RX pin of the HC-05 to TX pin of the Arduino
//You can use SoftwareSerial Library, but i dont recommend it for fast and long data transmission
//Otherwise you have to check Serial.available() > excepted number of bytes sent before reading the message
//There's no problem with hardware serial that comes with the arduino. It's perfect
//
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 255, 0), matrix.Color(255, 255, 0), matrix.Color(0, 0, 255) };




void setup()
{
  Serial.begin(9600);
  SerialBT.begin(9600);
  SerialBT.begin("ESP32 Badge");
  Serial.println("The device started, now you can pair it with bluetooth!");
  matrix.begin();
  matrix.setTextWrap(true);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
  Serial.begin(9600);
  //matrix.fillScreen(colors[0]);
  matrix.show();
  delay(3000);
  matrix.clear();
  matrix.setFont(&FontsFree_Net_uni05_534pt7b);
  matrix.fillScreen(0);
  matrix.setCursor(0, 4);
  matrix.setTextColor(colors[2]);

#if defined (__AVR_ATmega32U4__) // Leonardo
  while (!Serial) {}
#elif defined(__PIC32MX__)
  delay(1000);
#endif

  // Optional functions,  to add BEFORE connectWifi(...)
  //uduino.setPort(4222);   // default 4222
  //uduino.connectWifi("eduram", "zarazcipodam");
  //uduino.connectWifi("5G-Vectra-WiFi-16B63C", "bsh7nrhq4h0g2edo");
  //uduino.connectWifi("Bogdan", "mjku9075");

  uduino.addCommand("s", SetMode);
  uduino.addCommand("d", WritePinDigital);
  uduino.addCommand("a", WritePinAnalog);
  uduino.addCommand("rd", ReadDigitalPin);
  uduino.addCommand("r", ReadAnalogPin);
  uduino.addCommand("br", BundleReadPin);
  uduino.addCommand("b", ReadBundle);
  uduino.addCommand("sim", SetInverseMode);
  uduino.addCommand("spm", SetPartyMode);
  uduino.addCommand("spms", SetPartyModeSpeed);
  uduino.addCommand("lnt", LoadNewText);
  uduino.addCommand("cp", ChangePower);
  uduino.addCommand("ssa", SetScrollAlign);
  uduino.addCommand("sa", SetAlign);
  uduino.addCommand("lnst", LoadNewStaticText);
  uduino.addCommand("sam", SetAnimMode);
  uduino.addCommand("sss", SetScrollSpeed);
  uduino.addCommand("lat1", LoadAnimText1);
  uduino.addCommand("lat2", LoadAnimText2);
  uduino.addCommand("lat3", LoadAnimText3);

  Serial.println("Hello");
  //snprintf(dupaText,TABLEN,<IP KTOREODCZYTALEM>)
  //uduino.ge
  
}

void ReadBundle() {
  char *arg = NULL;
  char *number = NULL;
  number = uduino.next();
  int len = 0;
  if (number != NULL)
    len = atoi(number);
  for (int i = 0; i < len; i++) {
    uduino.launchCommand(arg);
  }
}

void SetMode() {
  int pinToMap = 100; //100 is never reached
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    pinToMap = atoi(arg);
  }
  int type;
  arg = uduino.next();
  if (arg != NULL)
  {
    //type = atoi(arg);
    //PinSetMode(pinToMap, type);
  }
}

// void PinSetMode(int pin, int type) {
//   switch (type) {
//     case 0: // Output
//       pinMode(pin, OUTPUT);
//       break;
//     case 1: // PWM
//       pinMode(pin, OUTPUT);
//       break;
//     case 2: // Analog
//       pinMode(pin, INPUT);
//       break;
//     case 3: // Input_Pullup
//       pinMode(pin, INPUT_PULLUP);
//       break;
//   }
// }

void WritePinAnalog() {
  int pinToMap = 100;
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    pinToMap = atoi(arg);
  }

  int valueToWrite;
  arg = uduino.next();
  if (arg != NULL)
  {
    valueToWrite = atoi(arg);
  }
}

void WritePinDigital() {
  int pinToMap = -1;
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
    pinToMap = atoi(arg);

  int writeValue;
  arg = uduino.next();
  if (arg != NULL && pinToMap != -1)
  {
    writeValue = atoi(arg);
    digitalWrite(pinToMap, writeValue);
  }
}

void ReadAnalogPin() {
  int pinToRead = -1;
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    pinToRead = atoi(arg);
    //if (pinToRead != -1)
      //printValue(pinToRead, analogRead(pinToRead));
  }
}

void ReadDigitalPin() {
  int pinToRead = -1;
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    pinToRead = atoi(arg);
  }

  //if (pinToRead != -1)
    //printValue(pinToRead, digitalRead(pinToRead));
}

void BundleReadPin() {
  int pinToRead = -1;
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    pinToRead = atoi(arg);
    //if (pinToRead != -1)
      //printValue(pinToRead, analogRead(pinToRead));
  }
}

// int replacechar(char *str, char orig, char rep) {
//     char *ix = str;
//     int n = 0;
//     while((ix = strchr(ix, orig)) != NULL) {
//         *ix++ = rep;
//         n++;
//     }
//     return n;
// }

void LoadNewText() {
  char *arg = NULL;
  //arg = uduino.next();
  //char *arg[SerialBT.readStringUntil("`").length() + 1];// = NULL;
  //Until('`').toCharArray(arg, SerialBT.readStringUntil('`').length() + 1);
  
    shouldBeStatic = true;  
    isAnim = false;
    isPartyMode = false;
    matrix.clear();
    matrix.setTextWrap(true);
    matrix.fillScreen(0);
    matrix.setCursor(0, 4);
    matrix.print(SerialBT.readStringUntil(defChar));
    //Serial.println(SerialBT.readString());
    matrix.show();
    SerialBT.flush();
    //Display.displayClear();
    //Display.displayScroll("Aaaaa",PA_RIGHT,PA_SCROLL_LEFT,100);
    //replacechar(dupaText, '_', ' ');
    // for (size_t i = 0; i < TABLEN; i++)
    // {
    //   if(arg[i] == '`'){
    //     arg[i] = ' ';
    //   }
    // }
    // strncpy(dupaText,arg,TABLEN);
    // Display.displayScroll(dupaText, state, scrollState, scrollSpeed);
    
  
  //Display.displayScroll("Hello there", PA_RIGHT, PA_SCROLL_LEFT, 100);
}

void LoadNewStaticText() {
  char *arg = NULL;
  arg = uduino.next();
  //arg = (char*)SerialBT.read();
  //Serial.write(arg);
  if (arg != NULL)
  {
    shouldBeStatic = false;
    Serial.println(arg);
    //replacechar(dupaText, '_', ' ');
    for (size_t i = 0; i < TABLEN; i++)
    {
      if(arg[i] == defChar){
        arg[i] = ' ';
      }
    }
    strncpy(staticText,arg,TABLEN);
  }
  //Display.displayScroll("Hello there", PA_RIGHT, PA_SCROLL_LEFT, 100);
}

void LoadAnimText1() {
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    Serial.println(arg);
    for (size_t i = 0; i < TABLEN; i++)
    {
      if(arg[i] == defChar){
        arg[i] = ' ';
      }
    }
    strncpy(animText1,arg,TABLEN);
  }
}

void LoadAnimText2() {
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    Serial.println(arg);
    for (size_t i = 0; i < TABLEN; i++)
    {
      if(arg[i] == defChar){
        arg[i] = ' ';
      }
    }
    strncpy(animText2,arg,TABLEN);
  }
}

void LoadAnimText3() {
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    Serial.println(arg);
    for (size_t i = 0; i < TABLEN; i++)
    {
      if(arg[i] == defChar){
        arg[i] = ' ';
      }
    }
    strncpy(animText3,arg,TABLEN);
  }
}

void ChangePower() {
    matrix.setBrightness(atoi(SerialBT.readStringUntil(defChar).c_str()));
    matrix.show();
    SerialBT.flush();
}

void SetInverseMode(){
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    isInverse = atoi(arg);
    Serial.println(arg);
  }
}

void SetPartyMode(){
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    isAnim = false;
    isPartyMode = atoi(arg);
  }
}

void SetAnimMode(){
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    isPartyMode = false;
    isAnim = atoi(arg);
  }
}

void SetScrollSpeed(){
    scrollSpeed = atoi(SerialBT.readStringUntil(defChar).c_str());
}

void SetPartyModeSpeed(){
  char *arg = NULL;
  arg = uduino.next();
  if (arg != NULL)
  {
    partySpeed = atoi(arg);
  }
}

void SetAlign() {
  char *arg = NULL;
  arg = uduino.next();
  // if (arg != NULL)
  // {
  //    stateVal = atoi(arg);
  //    switch (stateVal)
  //   {
  //   case 0:
  //   state = PA_CENTER;
  //     break;
  //   case 1:
  //   state = PA_RIGHT;
  //     break;
  //   case 2:
  //   state = PA_LEFT;
  //     break;
  //   }
  //   Display.setTextAlignment(state);
  // }
}

void SetScrollAlign() {
  char *arg = NULL;
  arg = uduino.next();
  // if (arg != NULL)
  // {
  //   Serial.println(arg);
  //   stateVal = atoi(arg);
  //   switch (stateVal)
  //   {
  //   case 0:
  //   scrollState = PA_SCROLL_LEFT;
  //     break;
  //   case 1:
  //   scrollState = PA_SCROLL_UP_LEFT;
  //     break;
  //   case 2:
  //   scrollState = PA_SCROLL_UP;
  //   break;
  //   case 3:
  //   scrollState = PA_SCROLL_UP_RIGHT;
  //   break;
  //   case 4:
  //   scrollState = PA_SCROLL_RIGHT;
  //   break;
  //   case 5:
  //   scrollState = PA_SCROLL_DOWN_RIGHT;
  //   break;
  //   case 6:
  //   scrollState = PA_SCROLL_DOWN;
  //   break;
  //   case 7:
  //   scrollState = PA_SCROLL_DOWN_LEFT;
  //   break;
  //   }
  // }
}

// char *arg = NULL;
//   arg = uduino.next();
//   if (arg != NULL)
//   {
//     Serial.println(arg);
//     for (size_t i = 0; i < TABLEN; i++)
//     {
//       if(arg[i] == '`'){
//         arg[i] = ' ';
//       }
//     }
//     strncpy(animText2,arg,TABLEN);
//   }

int line_pass = 0;
int x = matrix.width();

void ScrollText(){
  
  String scrollText = "hi";
  //scrollText = SerialBT.readStringUntil('`');
  
  
  matrix.print(F("RFduino"));
  int maxDisplaysment = scrollText.length() * 4 + matrix.width();
  // for (size_t i = 0; i < maxDisplaysment; i++)
  // {
    //if(++line_pass > matrix.width()) line_pass = 0;
    matrix.fillScreen(0);
    matrix.setCursor(x, 4);
    if(--x < -maxDisplaysment) {
      x = matrix.width();
    }
    matrix.show();
    delay(scrollSpeed);
  //}
}

void SetTextColor(){
  int r = atoi(SerialBT.readStringUntil(defChar).c_str());
  int g = atoi(SerialBT.readStringUntil(defChar).c_str());
  int b = atoi(SerialBT.readStringUntil(defChar).c_str());
  matrix.setTextColor(matrix.Color(r, g, b));
  matrix.show();
  SerialBT.flush();
}

void PaintPixels(){
  paint:
  int xPosition = atoi(SerialBT.readStringUntil(defChar).c_str());
  int yPosition = atoi(SerialBT.readStringUntil(defChar).c_str());
  int r = atoi(SerialBT.readStringUntil(defChar).c_str());
  int g = atoi(SerialBT.readStringUntil(defChar).c_str());
  int b = atoi(SerialBT.readStringUntil(defChar).c_str());
  Serial.println(xPosition);
  Serial.println(yPosition);
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  matrix.drawPixel(xPosition, yPosition, matrix.Color(r, g, b)); 
  matrix.show();

  if(SerialBT.readStringUntil(defChar) == "pp") goto paint;
  else SerialBT.flush();
}

String scrollTextIn = "";

void loop()
{
  //uduino.update();
  //char buffer[myString.length() + 1];

  String command = "";
  command = SerialBT.readStringUntil(defChar));
  Serial.print(command);
  if(command == "lnt") LoadNewText();
  if(command == "cp") ChangePower();
  if(command == "pp") PaintPixels();
  if(command == "cl"){ matrix.clear(); matrix.show();}
  if(command == "stc") SetTextColor();
  if(command == "sss") SetScrollSpeed();
  if(command == "st"){ 
    scrollTextIn = SerialBT.readStringUntil(defChar);
    matrix.print(scrollTextIn);
    shouldBeStatic = false;  
    isAnim = false;
    isPartyMode = false;
    matrix.setTextWrap(false);
  }
  //if(!shouldBeStatic) 
  // matrix.print(scrollTextIn);
  // int maxDisplaysment = scrollTextIn.length() * 4 + matrix.width();
  // Serial.print(maxDisplaysment);
  // Serial.print(scrollTextIn);
  // for (size_t i = 0; i < maxDisplaysment; i++)
  // {
    //if(++line_pass > matrix.width()) line_pass = 0;
    // matrix.fillScreen(0);
    // matrix.setCursor(x, 4);
    // if(--x < -maxDisplaysment) {
    //   x = matrix.width();
    // }
    matrix.show();
    delay(100);
  //}

  //delay(100);

// if (Serial.available()) {
//     SerialBT.write(Serial.read());
//     // Display.print(Serial.read());
//   }
//   if (SerialBT.available()) {
//     // char hub = SerialBT.read();
//     // Serial.write(hub);
//     // Display.print(hub);
//     // Display.setTextAlignment(state);
//   }
//   delay(100);

  // if (Display.displayAnimate()) {
  //    Display.displayReset();
  //  }
  
  // if(!shouldBeStatic){
  //   Display.print(staticText);
  //    Display.setInvert(isInverse);
  //    delay(partySpeed);
  //  }
  
  // if(isAnim){
  //   Display.print(animText1);
  //   delay(partySpeed);
  //   Display.print(animText2);
  //   delay(partySpeed);
  //   Display.print(animText3);
  //   delay(partySpeed);
  // }
  
  // if(isPartyMode){
  //   delay(partySpeed);
  //   Display.print("");
  //   delay(partySpeed);
  // }
}

void printValue(int pin, int targetValue) {
  uduino.print(pin);
  uduino.print(" "); //<- Todo : Change that with Uduino delimiter
  uduino.println(targetValue);
}

