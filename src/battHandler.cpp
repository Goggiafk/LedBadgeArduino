
#include <Arduino.h>

int g_voltage = 0;

static uint16_t measureBattVoltage()
{
    const float scale_a = 1.1;
    const float scale_b = 0.0;

    // 0 - 0V, 4096 - 3.3V
    int value = analogRead(15);

    // The battery voltage is divided by 1.5
    float LiPoVoltage = (value * 3.3 / 4096) * 1.5 * scale_a + scale_b;

    return (uint16_t)(LiPoVoltage * 1000.0);
}

void updateBattery()
{
    static const unsigned long REFRESH_INTERVAL = 100; // ms
    static unsigned long lastRefreshTime = 0;

    const int avgBuffLen = 16;

    static uint16_t avgBuff[avgBuffLen] = {0};
    static int buffPtr = 0;

    uint16_t currentVoltage = measureBattVoltage();

    if (millis() - lastRefreshTime >= REFRESH_INTERVAL)
    {
        lastRefreshTime += REFRESH_INTERVAL;
        avgBuff[buffPtr % avgBuffLen] = currentVoltage;

        // fill buffer on 1st pass
        if (buffPtr == 0)
        {
            for (int i = 0; i < avgBuffLen; i++)
            {
                avgBuff[i] = currentVoltage;
            }
        }

        buffPtr++;
        int voltageAcc = 0;

        for (int i = 0; i < avgBuffLen; i++)
        {
            voltageAcc += avgBuff[i];
        }

        g_voltage = voltageAcc / avgBuffLen;
    }
    if (g_voltage < 3400)
    {
    };
}

float getVoltage()
{
    return g_voltage / 1000.0;
}

String getBatteryText(float voltage){
    if (voltage > 4.65){
        return "CHG";
    } else if (voltage > 3.9 && voltage <= 4.65){
        return "High";
    } else if (voltage > 3.65 && voltage <= 3.9){
        return "Medium";
    } else {
        return "Low";
    }
}
