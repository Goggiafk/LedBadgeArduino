#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
//#include <Adafruit_TFTLCD.h>
#include <Fonts/CyberFont__1_8pt7b.h>
//#include "MAX17043.h"
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 22


//char * dupaText="Staly Tekst";
#define TABLEN 100
char dupaText[TABLEN];
char staticText[TABLEN];

bool shouldBeStatic = false;
bool isInverse = false;
bool isPartyMode = false;
int scrollAlign = 0;
int textAlign = 0;
int stateVal = 0;

int scrollSpeed = 100;
int partySpeed = 500;
int yTextOffset = 7;
int spacingBetweenLetters = 1;
bool isAnim = false;
char defChar = '`';
String text = "";

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
  SerialBT.begin("Badge1");
  Serial.println("The device started, now you can pair it with bluetooth!");
  matrix.begin();
  matrix.setTextWrap(true);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[2]);
  //matrix.setRotation(90);
  //matrix.fillScreen(colors[0]);
  //FuelGauge.begin();
  
  delay(3000);
  matrix.clear();
  matrix.setFont(&CyberFont__1_8pt7b);
  matrix.fillScreen(0);
  matrix.setCursor(0, yTextOffset);
  //float percentage = FuelGauge.percent();
  //float voltage = FuelGauge.voltage();
  //matrix.print(percentage);
  matrix.show();
  
#if defined (__AVR_ATmega32U4__) // Leonardo
  while (!Serial) {}
#elif defined(__PIC32MX__)
  delay(1000);
#endif
}


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
    matrix.setCursor(0, yTextOffset);
    
    //text = SerialBT.readStringUntil(defChar);
    text = SerialBT.readStringUntil(defChar);

    //matrix.setRotation(90);
    matrix.print(text);
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

void ChangePower() {
    matrix.setBrightness(atoi(SerialBT.readStringUntil(defChar).c_str()));
    matrix.show();
    SerialBT.flush();
}

void SetInverseMode(){
  char *arg = NULL;
  if (arg != NULL)
  {
    isInverse = atoi(arg);
    Serial.println(arg);
  }
}

void SetPartyMode(){
  char *arg = NULL;
  if (arg != NULL)
  {
    isAnim = false;
    isPartyMode = atoi(arg);
  }
}

void SetAnimMode(){
  char *arg = NULL;
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
  if (arg != NULL)
  {
    partySpeed = atoi(arg);
  }
}

void SetAlign() {
  //char *arg = NULL;
  //arg = uduino.next();

  String arg = "";
  arg = SerialBT.readStringUntil(defChar);

  if (arg != NULL)
  {
    arg;
    if(arg == "0")
        matrix.setCursor(0, yTextOffset);
    if(arg == "1")
        matrix.setCursor(16, yTextOffset);
    if(arg == "2")
        matrix.setCursor(32, yTextOffset);
     
     
     //Display.setTextAlignment(state);
  }
}

void SetScrollAlign() {
  char *arg = NULL;
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
  for (size_t i = 0; i < maxDisplaysment; i++)
  {
    if(++line_pass > matrix.width()) line_pass = 0;
    matrix.fillScreen(0);
    matrix.setCursor(x, yTextOffset);
    if(--x < -maxDisplaysment) {
      x = matrix.width();
    }
    matrix.show();
    delay(scrollSpeed);
  }
}

void SetTextColor(){
  int r = atoi(SerialBT.readStringUntil(defChar).c_str());
  int g = atoi(SerialBT.readStringUntil(defChar).c_str());
  int b = atoi(SerialBT.readStringUntil(defChar).c_str());
  matrix.setTextColor(matrix.Color(r, g, b));
  matrix.print(text);
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

void ShowBatteryState(){
  String batteryInfo = "";

  batteryInfo = "medium";

  SerialBT.println(batteryInfo);
  SerialBT.flush();
}

void ChangeName(){
  String newName = SerialBT.readStringUntil(defChar).c_str();

  
  //SerialBT.
  SerialBT.flush();
}

void loop()
{
  //uduino.update();
  //char buffer[myString.length() + 1];
  String command = "";
  command = SerialBT.readStringUntil(defChar);
  Serial.print(command);
  if(command == "lnt") LoadNewText();
  if(command == "cp") ChangePower();
  if(command == "pp") PaintPixels();
  if(command == "cl"){ matrix.clear(); matrix.show();}
  if(command == "stc") SetTextColor();
  if(command == "sss") SetScrollSpeed();
  if(command == "sa") SetAlign();
  if(command == "st"){ 
    //scrollTextIn = SerialBT.readStringUntil(defChar);
    //matrix.print(scrollTextIn);
    shouldBeStatic = false;  
    isAnim = false;
    isPartyMode = false;
    matrix.setTextWrap(false);
    ScrollText();
  }
  if(command == "sbs") ShowBatteryState();
  if(command == "cn") ChangeName();
  
  //LoadNewText();

  //if(!shouldBeStatic) 
  // matrix.print(scrollTextIn);
  // int maxDisplaysment = scrollTextIn.length() * 4 + matrix.width();
  // Serial.print(maxDisplaysment);
  // Serial.print(scrollTextIn);
  // for (size_t i = 0; i < maxDisplaysment; i++)
  // {
    //if(++line_pass > matrix.width()) line_pass = 0;
    // matrix.fillScreen(0);
    // matrix.setCursor(x, 6);
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


