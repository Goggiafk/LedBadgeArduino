
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include "BluetoothSerial.h"
#include "battHandler.h"
#include "pinDefs.h"
#include "blematrix.h"
#include "uniMatrix.h"

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

UniMatrix uniMatrix = UniMatrix(32, 8, &CyberFont__1_8pt7b);

void bleMatrixInit()
{
    uniMatrix.initialize();
    char badge_id[23];
    snprintf(badge_id, 11, "Badge-%llX", ESP.getEfuseMac());  
    SerialBT.begin(badge_id, false);

    uniMatrix.attachNeomatrixFrontend(&wsMatrix);
    uniMatrix.attachSerialFrontend([](const char* msg) {
        Serial.println(msg);
    });
}

void bleMatrixLoop(){
    if (SerialBT.available()) {
        ExecuteCommand();
    }
    redrawNonStaticIfNeeded();
}

void bleMatrixRedraw(){
    uniMatrix.redraw();
}

void LoadNewText()
{
    shouldBeStatic = true;
    isAnim = false;
    isPartyMode = false;
    uniMatrix.fillScreen(0);
    uniMatrix.setTextWrap(true);
    uniMatrix.setCursor(xTextOffset, yTextOffset);
    text = SerialBT.readStringUntil(defChar);
    uniMatrix.print(text);
    //uniMatrix.show();
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
    uniMatrix.print(text);
    //uniMatrix.show();
}

void ChangePower()
{
    wsMatrix.setBrightness(atoi(SerialBT.readStringUntil(defChar).c_str()));
    //uniMatrix.show();
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
            if (xTextOffset < uniMatrix.width() - 5)
                xTextOffset++;
        }
        if (arg == "1")
        {
            if (xTextOffset > -uniMatrix.width() + 5)
                xTextOffset--;
        }
        if (arg == "2")
        {
            xTextOffset = 0;
        }
        uniMatrix.setCursor(xTextOffset, yTextOffset);
        uniMatrix.fillScreen(0);
        uniMatrix.print(text);
        //uniMatrix.show();
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
int x = uniMatrix.width();

void ScrollText()
{
    uniMatrix.fillScreen(0);
    scrollTextIn = SerialBT.readStringUntil(defChar);
    shouldBeStatic = true;
    uniMatrix.print(scrollTextIn);
    shouldBeStatic = false;
    isAnim = false;
    isPartyMode = false;
    uniMatrix.setTextWrap(false);
    //uniMatrix.show();
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
    uniMatrix.setTextColor(wsMatrix.Color(r, g, b));
    uniMatrix.print(text);
    //uniMatrix.show();
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
    uniMatrix.drawPixel(xPosition, yPosition, wsMatrix.Color(r, g, b));
    //uniMatrix.show();

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
    uniMatrix.fillScreen(0);

    for (int i = 0; i < uniMatrix.width(); i++)
    {
        for (int j = 0; j < uniMatrix.height(); j++)
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
            uniMatrix.drawPixel(x, y, wsMatrix.Color(r, g, b));
            delay(5);
            //uniMatrix.show();
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
    uniMatrix.fillScreen(wsMatrix.Color(r, g, b));
    //uniMatrix.show();
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
        uniMatrix.fillScreen(0);
        //uniMatrix.show();
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
    textLength = scrollTextIn.length() * 5 + uniMatrix.width();
    int scrollLength = textLength;
    if (scrollAlign == 4)
        scrollLength += uniMatrix.width();
    for (size_t i = 0; i < scrollLength; i++)
    {
        if (shouldBeStatic == false)
        {
            if (SerialBT.available())
            {
                ExecuteCommand();
            }
            uniMatrix.fillScreen(0);
            uniMatrix.setCursor(x, yTextOffset);
            uniMatrix.print(scrollTextIn);
            if (scrollAlign == 0) --x;
            if (scrollAlign == 4) ++x;
            //uniMatrix.show();
            delay(scrollSpeed);
        } else break;
    }
    if (scrollAlign == 0) {
        x = uniMatrix.width();
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

    uniMatrix.fillScreen(0);
    uniMatrix.setTextColor(colors[0]);
    uniMatrix.setCursor(0, yTextOffset);
    uniMatrix.print(text);
    //uniMatrix.show();
}
