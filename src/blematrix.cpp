
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


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    32, 8, PIN_WS2812,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

BluetoothSerial SerialBT;


const uint16_t colors[] = {
    matrix.Color(255, 255, 0), matrix.Color(255, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255), matrix.Color(255, 0, 255)};

uint16_t matrixMap[8][32];



void init_matrixBackend()
{
    matrix.begin();
    matrix.setTextWrap(true);
    matrix.setBrightness(20);
    matrix.setTextColor(colors[2]);
    delay(50);

    matrix.clear();
    matrix.setFont(&CyberFont__1_8pt7b);
    matrix.fillScreen(0);
    matrix.setCursor(xTextOffset, yTextOffset);
    matrix.show();
}

void bleMatrixInit()
{
    // SerialBT.begin(9600);
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
    matrix.clear();
    matrix.setTextWrap(true);
    matrix.setCursor(xTextOffset, yTextOffset);
    text = SerialBT.readStringUntil(defChar);
    matrix.print(text);
    matrix.show();
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
    matrix.print(text);
    matrix.show();
}

void ChangePower()
{
    matrix.setBrightness(atoi(SerialBT.readStringUntil(defChar).c_str()));
    matrix.show();
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
            if (xTextOffset < matrix.width() - 5)
                xTextOffset++;
        }
        if (arg == "1")
        {
            if (xTextOffset > -matrix.width() + 5)
                xTextOffset--;
        }
        if (arg == "2")
        {
            xTextOffset = 0;
        }
        matrix.setCursor(xTextOffset, yTextOffset);
        matrix.clear();
        matrix.print(text);
        matrix.show();
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
int x = matrix.width();

void ScrollText()
{
    matrix.clear();
    scrollTextIn = SerialBT.readStringUntil(defChar);
    shouldBeStatic = true;
    matrix.print(scrollTextIn);
    shouldBeStatic = false;
    isAnim = false;
    isPartyMode = false;
    matrix.setTextWrap(false);
    matrix.show();
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
    matrix.setTextColor(matrix.Color(r, g, b));
    matrix.print(text);
    matrix.show();
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
    matrix.drawPixel(xPosition, yPosition, matrix.Color(r, g, b));
    matrix.show();

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
    for (size_t i = 0; i < matrix.width(); i++)
    {
        for (size_t j = 0; j < matrix.height(); j++)
        {
            matrixMap[j][i] = matrix.Color(0, 0, 0);
        }
    }
    matrix.clear();

    for (int i = 0; i < matrix.width(); i++)
    {
        for (int j = 0; j < matrix.height(); j++)
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

            Serial.println(r);
            Serial.println(g);
            Serial.println(b);
            Serial.println(" ");
            // matrixMap[x][y] = matrix.Color(r, g, b);
            matrix.drawPixel(x, y, matrix.Color(r, g, b));
            delay(5);
            matrix.show();
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
    matrix.fillScreen(matrix.Color(r, g, b));
    matrix.show();
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
        matrix.clear();
        matrix.show();
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
    textLength = scrollTextIn.length() * 5 + matrix.width();
    int scrollLength = textLength;
    if (scrollAlign == 4)
        scrollLength += matrix.width();
    for (size_t i = 0; i < scrollLength; i++)
    {
        if (shouldBeStatic == false)
        {
            if (SerialBT.available())
            {
                ExecuteCommand();
            }
            matrix.fillScreen(0);
            matrix.setCursor(x, yTextOffset);
            matrix.print(scrollTextIn);
            if (scrollAlign == 0) --x;
            if (scrollAlign == 4) ++x;
            matrix.show();
            delay(scrollSpeed);
        } else break;
    }
    if (scrollAlign == 0) {
        x = matrix.width();
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

    matrix.clear();
    matrix.setTextColor(colors[0]);
    matrix.setCursor(0, yTextOffset);
    matrix.print(text);
    matrix.show();
}
