#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "Button2.h"

#include "blematrix.h"
#include "pinDefs.h"
#include "battHandler.h"


void tap(Button2& btn);

Button2 buttonA, buttonB, buttonC;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif


void setup()
{
  Serial.begin(115200);
  
  Serial.println("The device started, now you can pair it with bluetooth!");
  
  bleMatrixInit();

  buttonA.begin(BUTTON_PIN_1);
  buttonA.setTapHandler(tap);

  buttonB.begin(BUTTON_PIN_2);
  buttonB.setTapHandler(tap);

  buttonC.begin(BUTTON_PIN_3);
  buttonC.setTapHandler(tap);
}


void tap(Button2 &btn)
{
    if (btn == buttonC)
    {
        Serial.print("tap1");
        drawBatteryText();
    }
    else if (btn == buttonA)
    {
        Serial.print("tap3");
        //TODO sleep the badge
    }
}


void loop()
{
  bleMatrixLoop();
    
  buttonA.loop();
  buttonB.loop();
  buttonC.loop();

  updateBattery();
}

