
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include "BluetoothSerial.h"
#include "battHandler.h"
#include "pinDefs.h"
#include "blematrix.h"

#include <CyberFont__1_8pt7b.h>

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

Adafruit_NeoMatrix wsMatrix = Adafruit_NeoMatrix(
    32, 8, PIN_WS2812,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

BluetoothSerial SerialBT;

const uint16_t colors[] = {
    wsMatrix.Color(255, 255, 0),
    wsMatrix.Color(255, 255, 0),
    wsMatrix.Color(0, 0, 255),
    wsMatrix.Color(255, 255, 255),
    wsMatrix.Color(255, 0, 255)
};

GFXcanvas16 canvas = GFXcanvas16(32, 8);

void init_matrixBackend()
{

    //initialize the WS canvas
    wsMatrix.begin();
    wsMatrix.clear();
    wsMatrix.setBrightness(20);
    wsMatrix.fillScreen(0);
    wsMatrix.show();

    //initialize the canvas object
    canvas.fillScreen(0);
    canvas.setFont(&CyberFont__1_8pt7b);
    canvas.setCursor(xTextOffset, yTextOffset);
}

uint16_t doubleBuffer[32][8];

void draw_matrixBackend(){
    wsMatrix.clear();
    memcpy(doubleBuffer, canvas.getBuffer(), sizeof(doubleBuffer));
    wsMatrix.drawRGBBitmap(0, 0, (uint16_t*)doubleBuffer, 32, 8);
    wsMatrix.show();
}

void bleMatrixInit()
{
    char badge_id[23];
    snprintf(badge_id, 11, "Badge-%llX", ESP.getEfuseMac());  
    SerialBT.begin(badge_id, false);

    init_matrixBackend();
}

void bleMatrixLoop(){
    if (SerialBT.available()) {
        ExecuteCommand();
    }
    redrawNonStaticIfNeeded();
}

void LoadNewText()
{
    shouldBeStatic = true;
    isAnim = false;
    isPartyMode = false;
    canvas.fillScreen(0);
    canvas.setTextWrap(true);
    canvas.setCursor(xTextOffset, yTextOffset);
    text = SerialBT.readStringUntil(defChar);
    canvas.print(text);
    //canvas.show();
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void AddNewText()
{
    char *arg = NULL;
    shouldBeStatic = true;
    isAnim = false;
    isPartyMode = false;
    text = "";
    text = SerialBT.readStringUntil(defChar);
    canvas.print(text);
    //canvas.show();
}

void ChangePower()
{
    wsMatrix.setBrightness(atoi(SerialBT.readStringUntil(defChar).c_str()));
    //canvas.show();
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void SetInverseMode()
{
    char *arg = NULL;
    if (arg != NULL)
    {
        isInverse = atoi(arg);
        Serial.println(arg);
    }
}

void SetPartyMode()
{
    char *arg = NULL;
    if (arg != NULL)
    {
        isAnim = false;
        isPartyMode = atoi(arg);
    }
}

void SetAnimMode()
{
    char *arg = NULL;
    if (arg != NULL)
    {
        isPartyMode = false;
        isAnim = atoi(arg);
    }
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void SetScrollSpeed()
{
    scrollSpeed = atoi(SerialBT.readStringUntil(defChar).c_str());
    shouldBeStatic = false;
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void SetPartyModeSpeed()
{
    char *arg = NULL;
    if (arg != NULL)
    {
        partySpeed = atoi(arg);
    }
}

void SetAlign()
{
    String arg = "";
    arg = atoi(SerialBT.readStringUntil(defChar).c_str());

    Serial.print(arg);
    if (arg != NULL)
    {
        if (arg == "0")
        {
            if (xTextOffset < canvas.width() - 5)
                xTextOffset++;
        }
        if (arg == "1")
        {
            if (xTextOffset > -canvas.width() + 5)
                xTextOffset--;
        }
        if (arg == "2")
        {
            xTextOffset = 0;
        }
        canvas.setCursor(xTextOffset, yTextOffset);
        canvas.fillScreen(0);
        canvas.print(text);
        //canvas.show();
    }
}

void SetScrollAlign()
{
    String arg = "";
    arg = SerialBT.readStringUntil(defChar);
    if (arg != NULL)
    {
        scrollAlign = atoi(arg.c_str());
    }
}

int line_pass = 0;
int x = canvas.width();

void ScrollText()
{
    canvas.fillScreen(0);
    scrollTextIn = SerialBT.readStringUntil(defChar);
    shouldBeStatic = true;
    canvas.print(scrollTextIn);
    shouldBeStatic = false;
    isAnim = false;
    isPartyMode = false;
    canvas.setTextWrap(false);
    //canvas.show();
    // SerialBT.flush();
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void SetTextColor()
{
    int r = atoi(SerialBT.readStringUntil(defChar).c_str());
    int g = atoi(SerialBT.readStringUntil(defChar).c_str());
    int b = atoi(SerialBT.readStringUntil(defChar).c_str());
    canvas.setTextColor(wsMatrix.Color(r, g, b));
    canvas.print(text);
    //canvas.show();
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void PaintPixels()
{
paint:
    int xPosition = atoi(SerialBT.readStringUntil(defChar).c_str());
    int yPosition = atoi(SerialBT.readStringUntil(defChar).c_str());
    int r = atoi(SerialBT.readStringUntil(defChar).c_str());
    int g = atoi(SerialBT.readStringUntil(defChar).c_str());
    int b = atoi(SerialBT.readStringUntil(defChar).c_str());
    char draw[20];
    snprintf(draw, 20, "#%02x%02x%2x @ %d/%d",r, g, b, xPosition, yPosition );
    Serial.println(draw);
    canvas.drawPixel(xPosition, yPosition, wsMatrix.Color(r, g, b));
    //canvas.show();

    if (SerialBT.readStringUntil(defChar) == "pp")
        goto paint;
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

int xS = 0;
int yS = 0;
// String loadedMap;
int loadedInt;

#define maxPixels 2560
char loadedMap[maxPixels] = {0};

void SaveDrawLoad()
{
    canvas.fillScreen(0);

    for (int i = 0; i < canvas.width(); i++)
    {
        for (int j = 0; j < canvas.height(); j++)
        {
            int x = atoi(SerialBT.readStringUntil(defChar).c_str());
            delay(5);
            int y = atoi(SerialBT.readStringUntil(defChar).c_str());
            delay(5);
            int r = atoi(SerialBT.readStringUntil(defChar).c_str());
            delay(5);
            int g = atoi(SerialBT.readStringUntil(defChar).c_str());
            delay(5);
            int b = atoi(SerialBT.readStringUntil(defChar).c_str());
    
            char draw[20];
            snprintf(draw, 20, "#%02x%02x%2x",r, g, b);
            Serial.println(draw);
            canvas.drawPixel(x, y, wsMatrix.Color(r, g, b));
            delay(5);
            //canvas.show();
        }
    }
}

void ChangeName()
{
    String newName = SerialBT.readStringUntil(defChar).c_str();
    if (SerialBT.available())
    {
        ExecuteCommand();
    }
}

void FillScreen()
{
    int r = atoi(SerialBT.readStringUntil(defChar).c_str());
    int g = atoi(SerialBT.readStringUntil(defChar).c_str());
    int b = atoi(SerialBT.readStringUntil(defChar).c_str());
    canvas.fillScreen(wsMatrix.Color(r, g, b));
    //canvas.show();
}

void ExecuteCommand()
{
    String command = "";
    command += SerialBT.readStringUntil(defChar);
    Serial.println(command);
    if (command == "lnt")
        LoadNewText();
    if (command == "cp")
        ChangePower();
    if (command == "pp")
        PaintPixels();
    if (command == "cl")
    {
        canvas.fillScreen(0);
        //canvas.show();
        if (SerialBT.available())
        {
            ExecuteCommand();
        }
    }
    if (command == "stc")
        SetTextColor();
    if (command == "sbs")
    {
        showBattery();
    }
    if (command == "sa")
        SetAlign();
    if (command == "ssa")
        SetScrollAlign();
    if (command == "st")
    {
        ScrollText();
        shouldBeStatic = false;
    }
    if (command == "sss")
        SetScrollSpeed();
    if (command == "cn")
        ChangeName();
    if (command == "sdl")
        SaveDrawLoad();
    if (command == "fc")
        FillScreen();
}

void showBattery() {
    float LiPoVoltage = getVoltage();

    String batteryInfo = "";
    if (LiPoVoltage > 4.65)
    {
        batteryInfo = "CHG";
    }
    else if (LiPoVoltage > 3.9 && LiPoVoltage <= 4.65)
    {
        batteryInfo = "high";
    }
    else if (LiPoVoltage > 3.65 && LiPoVoltage <= 3.9)
    {
        batteryInfo = "medium";
    }
    else if (LiPoVoltage <= 3.65)
    {
        batteryInfo = "low";
    }
    SerialBT.println(batteryInfo);
}

int textLength = 0;

void redrawNonStaticIfNeeded()
{
    if (shouldBeStatic){
        return;
    }
    textLength = scrollTextIn.length() * 5 + canvas.width();
    int scrollLength = textLength;
    if (scrollAlign == 4)
        scrollLength += canvas.width();
    for (size_t i = 0; i < scrollLength; i++)
    {
        if (shouldBeStatic == false)
        {
            if (SerialBT.available())
            {
                ExecuteCommand();
            }
            canvas.fillScreen(0);
            canvas.setCursor(x, yTextOffset);
            canvas.print(scrollTextIn);
            if (scrollAlign == 0) --x;
            if (scrollAlign == 4) ++x;
            //canvas.show();
            delay(scrollSpeed);
        } else break;
    }
    if (scrollAlign == 0) {
        x = canvas.width();
    }
    if (scrollAlign == 4){
        x = -textLength;
    }        
}

float savedBrightness = 0;
bool pin3clicked = false;
bool pin3canceled = true;

void drawBatteryText(){
    String text = getBatteryText(getVoltage());

    canvas.fillScreen(0);
    canvas.setTextColor(colors[0]);
    canvas.setCursor(0, yTextOffset);
    canvas.print(text);
    //canvas.show();
}
