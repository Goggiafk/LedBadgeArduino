#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Button2.h>

#include "bleMatrixDevice/bleMatrixDevice.h"
#include "pinDefs.h"


void tap(Button2& btn);

Button2 buttonA, buttonB, buttonC;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif


bleMatrixDevice1_0 deviceHAL(32,8, PIN_WS2812);

void drawMatrix(void * parameter)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    while (true)
    {
        // int time = millis();
        deviceHAL.redraw();
        // Serial.println(millis() - time);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin(115200);
    
    Serial.println("The device started, now you can pair it with bluetooth!");
    
    delay(2000);
    deviceHAL.init();

    buttonA.begin(BUTTON_PIN_1);
    buttonA.setTapHandler(tap);

    buttonB.begin(BUTTON_PIN_2);
    buttonB.setTapHandler(tap);

    buttonC.begin(BUTTON_PIN_3);
    buttonC.setTapHandler(tap);

    xTaskCreatePinnedToCore( drawMatrix, "drawMatrix",
        4096, NULL, 31, NULL , 1);
}


void tap(Button2 &btn)
{
    if (btn == buttonC)
    {
        Serial.print("tap1");
        deviceHAL.drawBattState();
    }
    else if (btn == buttonA)
    {
        Serial.print("tap3");
        //TODO sleep the badge
    }
}


void loop()
{
    deviceHAL.loop();
        
    buttonA.loop();
    buttonB.loop();
    buttonC.loop();
}

