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
  matrix.Color(255, 255, 0), matrix.Color(255, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255), matrix.Color(255, 0, 255) };


uint16_t matrixMap[8][32];

//char * dupaText="Staly Tekst";
#define TABLEN 100
char dupaText[TABLEN];
char staticText[TABLEN];

bool shouldBeStatic = true;
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
String scrollTextIn = "";



#define BUTTON_PIN_1 0
#define BUTTON_PIN_2 26
#define BUTTON_PIN_3 25
bool pin1clicked = false;
bool pin2clicked = false;
bool pin3clicked = false;

bool pin1canceled;
bool pin2canceled;
bool pin3canceled;

void setup()
{
  Serial.begin(9600);
  SerialBT.begin(9600);
  SerialBT.begin("Badge1");
  Serial.println("The device started, now you can pair it with bluetooth!");
  matrix.begin();
  matrix.setTextWrap(true);
  matrix.setBrightness(20);
  matrix.setTextColor(colors[2]);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
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
  shouldBeStatic = true;  
  isAnim = false;
  isPartyMode = false;
  matrix.clear();
  matrix.setTextWrap(true);
  //matrix.fillScreen(0);
  matrix.setCursor(0, yTextOffset);
  text = SerialBT.readStringUntil(defChar);
  matrix.print(text);
  matrix.show();
  if(SerialBT.available()){
    ExecuteCommand();
  }
}

void AddNewText() {
  char *arg = NULL;
  shouldBeStatic = true;  
  isAnim = false;
  isPartyMode = false;
  text = "";
  text = SerialBT.readStringUntil(defChar);
  matrix.print(text);
  matrix.show();
}

void ChangePower() {
  matrix.setBrightness(atoi(SerialBT.readStringUntil(defChar).c_str()));
  matrix.show();
  if(SerialBT.available()){
    ExecuteCommand();
  }
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
  if(SerialBT.available()){
    ExecuteCommand();
  }
}

void SetScrollSpeed(){
  scrollSpeed = atoi(SerialBT.readStringUntil(defChar).c_str());
  if(SerialBT.available()){
    ExecuteCommand();
  }
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

int line_pass = 0;
int x = matrix.width();

void ScrollText(){
  //matrix.clear();
  //scrollTextIn = SerialBT.readStringUntil(defChar);
  //matrix.print(scrollTextIn);
  shouldBeStatic = false;  
  isAnim = false;
  isPartyMode = false;
  matrix.setTextWrap(false);
  matrix.show();
  //SerialBT.flush();
   if(SerialBT.available()){
     ExecuteCommand();
   }
}

void SetTextColor(){
  int r = atoi(SerialBT.readStringUntil(defChar).c_str());
  int g = atoi(SerialBT.readStringUntil(defChar).c_str());
  int b = atoi(SerialBT.readStringUntil(defChar).c_str());
  matrix.setTextColor(matrix.Color(r, g, b));
  matrix.print(text);
  matrix.show();
  if(SerialBT.available()){
    ExecuteCommand();
  }
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
  if(SerialBT.available()){
    ExecuteCommand();
  }
}

int xS = 0;
int yS = 0;
void SaveDrawLoad(){


  for(int i = 0; i < matrix.width(); i++){
    for(int j = 0; j < matrix.height(); j++){
      int r = atoi(SerialBT.readStringUntil(defChar).c_str());
      int g = atoi(SerialBT.readStringUntil(defChar).c_str());
      int b = atoi(SerialBT.readStringUntil(defChar).c_str());
      //Serial.println(r);
      //Serial.println(g);
      //Serial.println(b);
      Serial.println(i + " " + j);
      //matrixMap[j][i] = matrix.Color(r, g, b);
      matrixMap[j][i] = matrix.Color(r, g, b);
      matrix.show();
    }
  }
  

  for(int i = 0; i < matrix.width(); i++){
    for(int j = 0; j < matrix.height(); j++){
        Serial.println(matrixMap[j][i]);
        matrix.drawPixel(i, j, matrixMap[j][i]);
        matrix.show();
    }
  }
  
  SerialBT.flush();
}

void ShowBatteryState(){
  String batteryInfo = "";

  batteryInfo = "medium";

  SerialBT.println(batteryInfo);
  if(SerialBT.available()){
    ExecuteCommand();
  }
}

void ChangeName(){
  String newName = SerialBT.readStringUntil(defChar).c_str();
  if(SerialBT.available()){
    ExecuteCommand();
  }
}

void FillScreen(){
  int r = atoi(SerialBT.readStringUntil(defChar).c_str());
  int g = atoi(SerialBT.readStringUntil(defChar).c_str());
  int b = atoi(SerialBT.readStringUntil(defChar).c_str());
  matrix.fillScreen(matrix.Color(r, g, b));
  matrix.show();
}

void ExecuteCommand(){
  String command = "";
  command += SerialBT.readStringUntil(defChar);
  Serial.println(command);
  if(command == "lnt") LoadNewText();
  if(command == "cp") ChangePower();
  if(command == "pp") PaintPixels();
  if(command == "cl"){ 
    matrix.clear(); 
    matrix.show(); 
    if(SerialBT.available()){
      ExecuteCommand();
    }
  }
  if(command == "stc") SetTextColor();
  if(command == "sss") SetScrollSpeed();
  if(command == "sa") SetAlign();
  if(command == "st") ScrollText();
  if(command == "sbs") ShowBatteryState();
  if(command == "cn") ChangeName();
  if(command == "sdl") SaveDrawLoad();
  if(command == "fc") FillScreen();
  if(command == "ant") AddNewText();
}

int value = 0;
float voltage;
float perc;
void showBattery(){
    value = analogRead(15);
    voltage = value * 5.0/1023;
    perc = map(voltage, 3.6, 4.2, 0, 100);
  
    Serial.print(perc, 0);
    matrix.print(perc);
    Serial.println("%");
    matrix.println("%");
    matrix.show();
    //delay(500);
}

float savedBrightness = 0;
void hideScreen(){
  savedBrightness = matrix.getBrightness();

  for (size_t i = savedBrightness; i > 1 ; i--)
  {
      matrix.setBrightness(i);
      matrix.show();
      delay(10);
  }
  
  pin1canceled = true;
}

void showScreen(){
  for (size_t i = 0; i < savedBrightness; i++)
  {
      matrix.setBrightness(i);
      matrix.show();
      delay(10);
  }
    
  pin1clicked = false;
}

int textLength = 0;
void loop()
{
  byte buttonState1 = digitalRead(BUTTON_PIN_1);
  byte buttonState2 = digitalRead(BUTTON_PIN_2);
  byte buttonState3 = digitalRead(BUTTON_PIN_3);

  if (buttonState1 == LOW) {
    hideScreen();  
  }
  
  if (buttonState2 == LOW) {
    showScreen();
  }
  if (buttonState3 == LOW) {

    showBattery();
  }
  
  ExecuteCommand();

  if(!shouldBeStatic) {
    textLength = scrollTextIn.length() * 5 + matrix.width();
    for (size_t i = 0; i < textLength; i++)
    {
      if(SerialBT.available()){
        ExecuteCommand();
        break;
      } else {
        matrix.fillScreen(0);
        matrix.setCursor(x, yTextOffset);
        matrix.print(scrollTextIn);
        --x;
        matrix.show();
        delay(scrollSpeed);
      }
    }
    x = matrix.width();
  }

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


