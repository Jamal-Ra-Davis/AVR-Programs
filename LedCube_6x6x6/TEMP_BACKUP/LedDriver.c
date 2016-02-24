#include <stdlib.h>
#include <avr/io.h>
#include "LedDriver.h"

LedDriver* makeLedDriver(uint8_t regClk, uint8_t latchClk, uint8_t redPin, uint8_t greenData, uint8_t blueData)
{
    LedDriver* ledDriver = (LedDriver*)malloc(sizeof(LedDriver));

    setLedDriver(ledDriver, regClk, latchClk, redData, greenData, blueData);

    return ledDriver;
}
void setLedDriver(LedDriver* ledDriver, uint8_t regClk, uint8_t latchClk, uint8_t redPin, uint8_t greenData, uint8_t blueData)
{
    ledDriver->regClk = regClk;
    ledDriver->latchClk = latchClk;
    ledDriver->redData = redData;
    ledDriver->greenData = greenData;
    ledDriver->blueData = blueData;
}


void sr_loadByte(LedDriver* ledDriver, uint8_t data)
{
    uint8_t mask;
    for (int i=7; i>=0; i--)
    //for (int i=0; i<7; i++)
    {
        SR_PORT &= ~(1 << ledDriver->regClk);//Triggers on rising edge of clock, must bring low first

        mask = 1 << i;
        if (mask & data)
            SR_PORT |= (1 << ledDriver->dataPin);
        else
            SR_PORT &= ~(1 << ledDriver->dataPin);

        SR_PORT |= (1 << ledDriver->regClk);//Loads data from data pin into shift reg on rising edge
    }
    SR_PORT &= ~(1 << ledDriver->latchClk);
    SR_PORT |= (1 << ledDriver->latchClk);    
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
        SR_PORT &= ~(1 << ledDriver->regClk);

        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & data[idx])
            SR_PORT |= (1 << ledDriver->dataPin);
        else
            SR_PORT &= ~(1 << ledDriver->dataPin);

        SR_PORT |= (1 << ledDriver->regClk);
    }
    SR_PORT &= ~(1 << ledDriver->latchClk);
    SR_PORT |= (1 << ledDriver->latchClk);
}
void sr_loadData(LedDriver* ledDriver, volatile uint8_t* data, int numBits)
{

    //if (numBits <= 0)
    //    return;

    //int i;//, numBits_ = numBits - 1;
    //int N = (numBits_)/8 + 1;
    //int N = (numBits - 1)/8 + 1;
    uint8_t N = ((numBits - 1) >> 3) + 1;//Number of bytes
    uint8_t idx;
    uint8_t bitSel;
    for (uint8_t i=0; i<numBits; i++)
    {
        SR_PORT &= ~(1 << ledDriver->regClk);

        //int idx = N - 1 - i/8;
        idx = N - 1 - (i >> 3);
        bitSel = i - (N - 1 - idx)*8;
        //int bitSel = i - ((N - 1 - idx) << 3);        

        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & data[idx])
            SR_PORT |= (1 << ledDriver->dataPin);
        else
            SR_PORT &= ~(1 << ledDriver->dataPin);

        SR_PORT |= (1 << ledDriver->regClk);
    }
    SR_PORT &= ~(1 << ledDriver->latchClk);
    SR_PORT |= (1 << ledDriver->latchClk);
}
void sr_loadData_alt(LedDriver* ledDriver, volatile uint8_t* data, int numBits)
{
    uint8_t regClk = ledDriver->regClk;
    uint8_t dataPin = ledDriver->dataPin;
    uint8_t latchClk = ledDriver->regClk;

    uint8_t i, j;
    uint8_t N = ((numBits - 1) >> 3) + 1;


    for (i = N - 1; (i != 255) && numBits; i--)
    {
        for (j = 0; (j<8) && numBits; j++, numBits--)
        {
            SR_PORT &= ~(1 << regClk);
            
            if ((1 << j) & data[i])
                SR_PORT |= (1 << dataPin);
            else
                SR_PORT &= ~(1 << dataPin);
            SR_PORT |= (1 << regClk);
        }
    }
    SR_PORT &= ~(1 << ledDriver->latchClk);
    SR_PORT |= (1 << ledDriver->latchClk);
}

