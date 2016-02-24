#include <stdlib.h>
#include <avr/io.h>
#include "LedDriver.h"
#include "FrameBuffer.h"


LedDriver* makeLedDriver(uint8_t regClk, uint8_t latchClk, uint8_t redData, uint8_t greenData, uint8_t blueData)
{
    LedDriver* ledDriver = (LedDriver*)malloc(sizeof(LedDriver));

    setLedDriver(ledDriver, regClk, latchClk, redData, greenData, blueData);

    return ledDriver;
}
void setLedDriver(LedDriver* ledDriver, uint8_t regClk, uint8_t latchClk, uint8_t redData, uint8_t greenData, uint8_t blueData)
{
    ledDriver->regClk = regClk;
    ledDriver->latchClk = latchClk;
    ledDriver->redData = redData;
    ledDriver->greenData = greenData;
    ledDriver->blueData = blueData;
}

GroundDriver* makeGroundDriver(uint8_t regClk, uint8_t latchClk, uint8_t data)
{
    GroundDriver* groundDriver = (GroundDriver*)malloc(sizeof(GroundDriver));
    setGroundDriver(groundDriver, regClk, latchClk, data);
    return groundDriver;
}
void setGroundDriver(GroundDriver* groundDriver, uint8_t regClk, uint8_t latchClk, uint8_t data)
{
    groundDriver->regClk = regClk;
    groundDriver->latchClk = latchClk;
    groundDriver->data = data;

    enableGroundDriver(groundDriver);
}
void enableGroundDriver(GroundDriver* groundDriver)
{
    //Enable ground shift register pins as outputs
    GND_DDR |= (1 << groundDriver->data)|(1 << groundDriver->regClk)|(1 << groundDriver->latchClk);
    GND_PORTS &= ~((1 << groundDriver->data)|(1 << groundDriver->regClk)|(1 << groundDriver->latchClk));

    //Enable ground enable pin as output
    GND_EN_DDR |= (1 << GND_EN_PIN);
}
void disableGroundDriver(GroundDriver* groundDriver)
{
    //Disable ground shift register pins as outputs
    GND_DDR &= ~((1 << groundDriver->data)|(1 << groundDriver->regClk)|(1 << groundDriver->latchClk));

    //Disable ground enable pin as output
    GND_EN_DDR &= ~(1 << GND_EN_PIN);
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
            SR_PORT |= (1 << ledDriver->redData);
        else
            SR_PORT &= ~(1 << ledDriver->redData);

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
            SR_PORT |= (1 << ledDriver->redData);
        else
            SR_PORT &= ~(1 << ledDriver->redData);

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
            SR_PORT |= (1 << ledDriver->redData);
        else
            SR_PORT &= ~(1 << ledDriver->redData);

        SR_PORT |= (1 << ledDriver->regClk);
    }
    SR_PORT &= ~(1 << ledDriver->latchClk);
    SR_PORT |= (1 << ledDriver->latchClk);
}
void sr_loadData_alt(LedDriver* ledDriver, volatile uint8_t data[3][FBUF_REGS], int numBits)
{
    uint8_t regClk = ledDriver->regClk;
    uint8_t latchClk = ledDriver->latchClk;

    uint8_t redData = ledDriver->redData;
    uint8_t greenData = ledDriver->greenData;
    uint8_t blueData = ledDriver->blueData; 

    uint8_t i, j;
    uint8_t N = ((numBits - 1) >> 3) + 1;


    for (i = N - 1; (i != 255) && numBits; i--)
    {
        for (j = 0; (j<8) && numBits; j++, numBits--)
        {
            SR_PORT &= ~(1 << regClk);
            
            if ((1 << j) & data[0][i])
                SR_PORT |= (1 << redData);
            else
                SR_PORT &= ~(1 << redData);
            
            if ((1 << j) & data[1][i])
                SR_PORT |= (1 << greenData);
            else
                SR_PORT &= ~(1 << greenData);

            if ((1 << j) & data[2][i])
                SR_PORT |= (1 << blueData);
            else
                SR_PORT &= ~(1 << blueData);

            SR_PORT |= (1 << regClk);
        }
    }
    SR_PORT &= ~(1 << latchClk);
    SR_PORT |= (1 << latchClk);
}

void initGroundRegister(GroundDriver* groundDriver)
{
    int i;
    //Loads ground register with zeros
    
    //Loads FBUF_SZ 0's into shift register
    GND_PORTS &= ~(1 << groundDriver->data);
    for (i=0; i<FBUF_SZ; i++)
    {
        GND_PORTS &= ~(1 << groundDriver->regClk);
        GND_PORTS |= (1 << groundDriver->regClk);
    }

    GND_PORTS &= ~(1 << groundDriver->latchClk);
    GND_PORTS |= (1 << groundDriver->latchClk);

    //Enables ground enable pin for ground planes control
    GND_EN_PORT &= ~(1 << GND_EN_PIN);
}
void sr_incrementGround(GroundDriver* groundDriver, FrameBuffer* framebuffer)
{
    GND_PORTS &= ~(1 << groundDriver->regClk);
    //If current level == 0, set data line HIGH
    if (framebuffer->currLevel == 0)
    {
        GND_PORTS |= (1 << groundDriver->data);
    }
    //Otherwise set LOW
    else
    {
        GND_PORTS &= ~(1 << groundDriver->data);    
    }
    //Clock in data bit
    GND_PORTS |= (1 << groundDriver->regClk);

    //Latch out data
    GND_PORTS &= ~(1 << groundDriver->latchClk);
    GND_PORTS |= (1 << groundDriver->latchClk);

    framebuffer->currLevel++;
    if (framebuffer->currLevel >= FBUF_SZ)
        framebuffer->currLevel = 0;
}
