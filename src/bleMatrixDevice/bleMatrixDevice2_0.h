
#pragma once

#include <BluetoothSerial.h>
#include "uniMatrix.h"

#include <Adafruit_NeoMatrix.h>
#include <CyberFont__1_8pt7b.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include "bleMatrixDevice/bleMatrixDevice.h"

class bleMatrixDevice2_0 : private virtual bleMatrixDevice
{
    // this class is a HAL for cyberbadge 1.0 and 1.1

    // it handles ws2812 matrix and bluetooth classic interface
    public:
    bleMatrixDevice2_0(int _w, int _h, int _pin)
    : uniMatrix(_w, _h, &CyberFont__1_8pt7b),
    wsMatrix(_w, _h, _pin,
        NEO_MATRIX_TOP + NEO_MATRIX_LEFT + 
        NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
        NEO_GRB + NEO_KHZ800),
    SerialBT() {
        //nothing
    }

    void init(){
        uniMatrix.initialize();
        char badge_id[23];
        snprintf(badge_id, 11, "Badge-%llX", ESP.getEfuseMac());  
        SerialBT.begin(badge_id, false);

        uniMatrix.attachNeomatrixFrontend(&wsMatrix);
        uniMatrix.attachSerialFrontend([](const char* msg) {
            Serial.println(msg);
        });
    }

    void loop(){
        if (SerialBT.available()) {
            ExecuteCommand();
        }
        redrawNonStaticIfNeeded();
        updateBattery();
    }


    void redraw(){
        uniMatrix.redraw();
    }


    void setText(uint16_t color, int brightness, int scrollSpeed, String text){
        if ((brightness > 0) && (brightness < 255)) {
            wsMatrix.setBrightness(brightness);
        }
        if (color != 0x00){
            uniMatrix.setTextColor(color);
        }
        (void)scrollSpeed;

        uniMatrix.fillScreen(0);
        uniMatrix.setCursor(xTextOffset, yTextOffset);
        uniMatrix.print(text);
        //uniMatrix.show();
    }



    private:

    const int yTextOffset = 7;
    const char defChar = '`';

    int scrollSpeed = 100;
    int xTextOffset = 0;
    bool isAnim = false;

    UniMatrix uniMatrix;
    Adafruit_NeoMatrix wsMatrix;
    BluetoothSerial SerialBT;
};






    // void ExecuteCommand()
    // {
    //     String command = "";
    //     command += SerialBT.readStringUntil(defChar);
    //     Serial.println(command);
    //     if (command == "lnt")
    //         LoadNewText();
    //     if (command == "cp")
    //         ChangePower();
    //     if (command == "pp")
    //         PaintPixels();
    //     if (command == "cl")
    //     {
    //         uniMatrix.fillScreen(0);
    //         if (SerialBT.available())
    //         {
    //             ExecuteCommand();
    //         }
    //     }
    //     if (command == "stc")
    //         SetTextColor();
    //     if (command == "sbs")
    //     {
    //         showBattery();
    //     }
    //     if (command == "sa")
    //         SetAlign();
    //     if (command == "ssa")
    //         SetScrollAlign();
    //     if (command == "st")
    //     {
    //         ScrollText();
    //         shouldBeStatic = false;
    //     }
    //     if (command == "sss")
    //         SetScrollSpeed();
    //     if (command == "cn")
    //         ChangeName();
    //     if (command == "sdl")
    //         SaveDrawLoad();
    //     if (command == "fc")
    //         FillScreen();
    // }




    // void PaintPixels()
    // {
    // paint:
    //     int xPosition = atoi(SerialBT.readStringUntil(defChar).c_str());
    //     int yPosition = atoi(SerialBT.readStringUntil(defChar).c_str());
    //     int r = atoi(SerialBT.readStringUntil(defChar).c_str());
    //     int g = atoi(SerialBT.readStringUntil(defChar).c_str());
    //     int b = atoi(SerialBT.readStringUntil(defChar).c_str());
    //     char draw[20];
    //     snprintf(draw, 20, "#%02x%02x%2x @ %d/%d",r, g, b, xPosition, yPosition );
    //     Serial.println(draw);
    //     uniMatrix.drawPixel(xPosition, yPosition, wsMatrix.Color(r, g, b));
    //     //uniMatrix.show();

    //     if (SerialBT.readStringUntil(defChar) == "pp")
    //         goto paint;
    //     if (SerialBT.available())
    //     {
    //         ExecuteCommand();
    //     }
    // }

    // void showBattery() {
    //     float LiPoVoltage = getVoltage();

    //     String batteryInfo = "";
    //     if (LiPoVoltage > 4.65)
    //     {
    //         batteryInfo = "CHG";
    //     }
    //     else if (LiPoVoltage > 3.9 && LiPoVoltage <= 4.65)
    //     {
    //         batteryInfo = "high";
    //     }
    //     else if (LiPoVoltage > 3.65 && LiPoVoltage <= 3.9)
    //     {
    //         batteryInfo = "medium";
    //     }
    //     else if (LiPoVoltage <= 3.65)
    //     {
    //         batteryInfo = "low";
    //     }
    //     SerialBT.println(batteryInfo);
    // }

    // void redrawNonStaticIfNeeded()
    // {
    //     if (shouldBeStatic){
    //         return;
    //     }
    //     int textLength = scrollTextIn.length() * 5 + uniMatrix.width();
    //     int scrollLength = textLength;
    //     if (scrollAlign == 4)
    //         scrollLength += uniMatrix.width();
    //     for (size_t i = 0; i < scrollLength; i++)
    //     {
    //         if (shouldBeStatic == false)
    //         {
    //             if (SerialBT.available())
    //             {
    //                 ExecuteCommand();
    //             }
    //             uniMatrix.fillScreen(0);
    //             uniMatrix.setCursor(x, yTextOffset);
    //             uniMatrix.print(scrollTextIn);
    //             if (scrollAlign == 0) --x;
    //             if (scrollAlign == 4) ++x;
    //             //uniMatrix.show();
    //             delay(scrollSpeed);
    //         } else break;
    //     }
    //     if (scrollAlign == 0) {
    //         x = uniMatrix.width();
    //     }
    //     if (scrollAlign == 4){
    //         x = -textLength;
    //     }        
    // }

    // void drawBattState(){
    //     String text = getBatteryText(getVoltage());

    //     uniMatrix.fillScreen(0);
    //     uniMatrix.setTextColor(255, 255, 0);
    //     uniMatrix.setCursor(0, yTextOffset);
    //     uniMatrix.print(text);
    // }