#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
//#include <Adafruit_TFTLCD.h>
#include <CyberFont__1_8pt7b.h>
#include "Button2.h"
//#include "MAX17043.h"
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 22

#include "BluetoothSerial.h"

void tap(Button2& btn);
void ExecuteCommand();
void showBattery();

Button2 buttonA, buttonB, buttonC;

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


bool shouldBeStatic = true;
bool isInverse = false;
bool isPartyMode = false;
int scrollAlign = 0;
int textAlign = 0;
int stateVal = 0;

int scrollSpeed = 100;
int partySpeed = 500;
int yTextOffset = 7;
int xTextOffset = 0;
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

bool pin1canceled = true;
bool pin2canceled = true;
bool pin3canceled = true;

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
  
  
  //matrix.setRotation(90);
  //matrix.fillScreen(colors[0]);
  //FuelGauge.begin();
  
  delay(50);
  buttonA.begin(BUTTON_PIN_1);
  //buttonA.setClickHandler(click);
  buttonA.setTapHandler(tap);

  buttonB.begin(BUTTON_PIN_2);
  //buttonB.setClickHandler(click);
  buttonB.setTapHandler(tap);

  buttonC.begin(BUTTON_PIN_3);
  //buttonC.setClickHandler(click);
  buttonC.setTapHandler(tap);

  matrix.clear();
  matrix.setFont(&CyberFont__1_8pt7b);
  matrix.fillScreen(0);
  matrix.setCursor(xTextOffset, yTextOffset);
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
  shouldBeStatic = true;  
  isAnim = false;
  isPartyMode = false;
  matrix.clear();
  matrix.setTextWrap(true);
  matrix.setCursor(xTextOffset, yTextOffset);
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
  shouldBeStatic = false;
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
  String arg = "";
  arg = atoi(SerialBT.readStringUntil(defChar).c_str());

  Serial.print(arg);
  if (arg != NULL)
  {
    if(arg == "0"){
      if(xTextOffset < matrix.width()-5)
        xTextOffset++;
    }
    if(arg == "1"){
      if(xTextOffset > -matrix.width() + 5)
        xTextOffset--;
    }
    if(arg == "2"){
        xTextOffset = 0;
    }
    matrix.setCursor(xTextOffset, yTextOffset);
    matrix.clear();
    matrix.print(text);
    matrix.show();
  }
}

void SetScrollAlign() {
  String arg = "";
  arg = SerialBT.readStringUntil(defChar);
  if (arg != NULL)
  {
    scrollAlign = atoi(arg.c_str());
  }
}

int line_pass = 0;
int x = matrix.width();

void ScrollText(){
  matrix.clear();
  scrollTextIn = SerialBT.readStringUntil(defChar);
  shouldBeStatic = true;
  matrix.print(scrollTextIn);
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
//String loadedMap;
int loadedInt;


#define maxPixels 2560
char loadedMap[maxPixels] = {0};

void SaveDrawLoad(){
  // for (size_t i = 0; i < matrix.width(); i++)
  // {
  //   for (size_t j = 0; j < matrix.height(); j++)
  //   {
  //     matrixMap[j][i] = matrix.Color(0, 0, 0);
  //   }
  // }
  matrix.clear();  

for(int i = 0; i < matrix.width(); i++){
    for(int j = 0; j < matrix.height(); j++){
      int x = atoi(SerialBT.readStringUntil(defChar).c_str());
      delay(5); 
      int y = atoi(SerialBT.readStringUntil(defChar).c_str());
      delay(5); 
      int r = atoi(SerialBT.readStringUntil(defChar).c_str());
      delay(5); 
      int g = atoi(SerialBT.readStringUntil(defChar).c_str());
      delay(5); 
      int b = atoi(SerialBT.readStringUntil(defChar).c_str());
      

      Serial.println(r);
      Serial.println(g);
      Serial.println(b);
      Serial.println(" ");
      //matrixMap[x][y] = matrix.Color(r, g, b);
      matrix.drawPixel(x, y, matrix.Color(r, g, b));
      delay(5);
      matrix.show();
    }
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

String batteryCommand = "";
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
  if(command == "sbs"){ 
    showBattery(); 
    batteryCommand = command;
  } 
  if(command == "sa") SetAlign();
  if(command == "ssa") SetScrollAlign();
  if(command == "st"){ 
    ScrollText();
    shouldBeStatic = false;
  }
  if(command == "sss") SetScrollSpeed();
  if(command == "cn") ChangeName();
  if(command == "sdl") SaveDrawLoad();
  if(command == "fc") FillScreen();
  //if(command == "ant") AddNewText();
   
}

void showBattery(){
    const float scale_a = 1.1;
    const float scale_b = 0.0;

    //0 - 0V, 4096 - 3.3V
    int value = analogRead(15);

    //The battery voltage is divided by 1.5
    float LiPoVoltage = (value * 3.3/4096)*1.5*scale_a + scale_b;
    String batteryInfo = "";
    //matrix.fillScreen(0);
    //matrix.setCursor(0, yTextOffset);
    if (LiPoVoltage > 4.8){
      batteryInfo = "CHG";
    } else if(LiPoVoltage > 3.9 && LiPoVoltage <= 4.8){
      batteryInfo = "high";
    } else 
    if(LiPoVoltage > 3.65 && LiPoVoltage <= 3.9){
      batteryInfo = "medium";
    } else 
    if(LiPoVoltage <= 3.65){
      batteryInfo = "low";
    }
    SerialBT.println(batteryInfo);
    //matrix.print(batteryInfo);
    matrix.show();
    

    if(SerialBT.available()){
      ExecuteCommand();
    }
     
}

int textLength = 0;

void loop()
{
  if(SerialBT.available()){
    ExecuteCommand();
  }

  if(shouldBeStatic == false) {
    textLength = scrollTextIn.length() * 5 + matrix.width();
    int scrollLength = textLength;
    if(scrollAlign == 4)
      scrollLength += matrix.width();
    for (size_t i = 0; i < scrollLength; i++)
    {
      if(shouldBeStatic == false) {
        if(SerialBT.available()){
          ExecuteCommand();
        }
        matrix.fillScreen(0);
        matrix.setCursor(x, yTextOffset);
        matrix.print(scrollTextIn);
        if(scrollAlign == 0)
          --x;
        if(scrollAlign == 4)
          ++x;
        matrix.show();
        delay(scrollSpeed);
    } else
    break;
    }
    if(scrollAlign == 0)
      x = matrix.width();
    if(scrollAlign == 4)
      x = -textLength;
  }
   

  buttonA.loop();
  buttonB.loop();
  buttonC.loop();

  

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
float savedBrightness = 0;
void tap(Button2& btn) {
  //  if (btn == buttonA) {
  //    Serial.print("tap1");
  //   } else if (btn == buttonB) {
  //     Serial.print("tap2");
  //   } else if (btn == buttonC) {
  //     Serial.print("tap3");
  //   if(pin3canceled){
  //     if(pin3clicked == false){ 
  //       savedBrightness = matrix.getBrightness();
  //       pin3clicked = true;
  //       pin3canceled = false;
  //       for (size_t i = savedBrightness; i > 1 ; i--)
  //        {
  //          matrix.setBrightness(i);
  //          delay(1);
  //          matrix.show();
  //       }
  //       matrix.setBrightness(1);
  //       pin3canceled = true;
  //     }
  //     else{ 
  //       pin3clicked = false;
  //       pin3canceled = false;
  //       for (size_t i = 1; i < savedBrightness; i++)
  //       {
  //         matrix.setBrightness(i);
  //         delay(1);
  //         matrix.show();
  //       }
  //       matrix.setBrightness(savedBrightness);
  //       pin3canceled = true;
        
  //     }
  //   }
  //      //showScreen();
  //   }
    
}


