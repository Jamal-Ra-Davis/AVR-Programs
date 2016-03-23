#include <stdlib.h>
#include <avr/io.h>
#include "LedDriver.h"

LedDriver* makeLedDriver(uint8_t regClk, uint8_t latchClk, uint8_t dataPin)
{
    LedDriver* ledDriver = (LedDriver*)malloc(sizeof(LedDriver));

    setLedDriver(ledDriver, regClk, latchClk, dataPin);

    return ledDriver;
}
void setLedDriver(LedDriver* ledDriver, uint8_t regClk, uint8_t latchClk, uint8_t dataPin)
{
    ledDriver->regClk = regClk;
    ledDriver->latchClk = latchClk;
    ledDriver->dataPin = dataPin;
}


void sr_loadByte(LedDriver* ledDriver, uint8_t data)
{
    uint8_t mask;
    for (int i=7; i>=0; i--)
    {
        LEDS_PORT &= ~(1 << ledDriver->regClk);//Triggers on rising edge of clock, must bring low first

        mask = 1 << i;
        if (mask & data)
            LEDS_PORT |= (1 << ledDriver->dataPin);
        else
            LEDS_PORT &= ~(1 << ledDriver->dataPin);

        LEDS_PORT |= (1 << ledDriver->regClk);//Loads data from data pin into shift reg on rising edge
    }
    LEDS_PORT &= ~(1 << ledDriver->latchClk);
    LEDS_PORT |= (1 << ledDriver->latchClk);    
}
//Old function, want to reverse order of load
void sr_loadData_old(LedDriver* ledDriver, volatile uint8_t* data, int numBits)
{
    if (numBits <= 0)
        return;
    int i, numBits_ = numBits - 1;
    int N = (numBits_)/8 + 1;
    for (i=numBits_; i>=0; i--)
    {
        LEDS_PORT &= ~(1 << ledDriver->regClk);

        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & data[idx])
            LEDS_PORT |= (1 << ledDriver->dataPin);
        else
            LEDS_PORT &= ~(1 << ledDriver->dataPin);

        LEDS_PORT |= (1 << ledDriver->regClk);
    }
    LEDS_PORT &= ~(1 << ledDriver->latchClk);
    LEDS_PORT |= (1 << ledDriver->latchClk);
}
void sr_loadData(LedDriver* ledDriver, volatile uint8_t* data, int numBits)
{
    if (numBits <= 0)
        return;
    int i, numBits_ = numBits - 1;
    int N = (numBits_)/8 + 1;
    for (i=0; i<numBits; i++)
    {
        LEDS_PORT &= ~(1 << ledDriver->regClk);

        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & data[idx])
            LEDS_PORT |= (1 << ledDriver->dataPin);
        else
            LEDS_PORT &= ~(1 << ledDriver->dataPin);

        LEDS_PORT |= (1 << ledDriver->regClk);
    }
    LEDS_PORT &= ~(1 << ledDriver->latchClk);
    LEDS_PORT |= (1 << ledDriver->latchClk);
}


