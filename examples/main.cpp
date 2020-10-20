#include <Arduino.h>
#include <Wire.h>
#include "fm31256.h"

FM31256 fm;

#define WD_CHECK_PIN 4

void setup()
{
    pinMode(WD_CHECK_PIN, INPUT_PULLUP);

    Wire.setClock(1000000);
    Wire.begin();

    Serial.begin(9600);
    while (!Serial); 

    fm.watchDog(false);
    fm.setWatchDogValue(15);
    fm.watchDog(true);

    fm.enableOsc();

    // for (uint16_t i = 1000; i < 2000; i++)
    // {
    //     byte ret = fm.writeWord(i * 2, i);
    //     if (ret != 0)
    //     {
    //         Serial.print(i);Serial.print("(");Serial.print(ret);Serial.print(") ");
    //     }
    // }

    fm.writeCounter1(1153);
    fm.writeCounter2(2264);


    fm.beginUpdateRTC();
    fm.writeRtcRegister(REG_YEAR, 20);
    fm.writeRtcRegister(REG_MONTH, 10);
    fm.writeRtcRegister(REG_DAY, 18);
    fm.writeRtcRegister(REG_HOURS, 11);
    fm.writeRtcRegister(REG_MINUTES, 11);
    fm.writeRtcRegister(REG_SECONDS, 0);
    fm.endUpdateRTC();

    uint16_t val;
    for (uint16_t i = 1000; i < 2000; i++)
    {
        byte ret = fm.readWord(i * 2, &val);
        if (ret != 0)
        {
            Serial.print(i);Serial.print("(");Serial.print(ret);Serial.print(") ");
        }
        else
        {
            Serial.print(val); Serial.print(" ");
        }
    }
}


void loop()
{
    Serial.print(" CNT1="); Serial.println(fm.readCounter1());
    Serial.print(" CNT2="); Serial.println(fm.readCounter2());
    Serial.print(" WD PIN = "); Serial.println(digitalRead(WD_CHECK_PIN));

    Serial.print(millis());
    fm.prepareReadRTC();
    Serial.print(" "); Serial.print(fm.readRtcRegister(REG_YEAR));
    Serial.print(":"); Serial.print(fm.readRtcRegister(REG_MONTH));
    Serial.print(":"); Serial.print(fm.readRtcRegister(REG_DAY));
    Serial.print(" "); Serial.print(fm.readRtcRegister(REG_HOURS));
    Serial.print(":"); Serial.print(fm.readRtcRegister(REG_MINUTES));
    Serial.print(":"); Serial.println(fm.readRtcRegister(REG_SECONDS));

    if (millis() < 45000)
        fm.resetWatchDog();


    delay(1000);  
}
