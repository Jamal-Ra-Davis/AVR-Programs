#include <avr/io.h>
#include <util/delay.h>

#define OUT_DDR DDRD
#define OUT_PORT PORTD
#define REG_CLK PD5
#define LATCH_CLK PD6
#define DATA_PIN PD7
#define BUF_SZ 4

void POVDisplay(uint8_t oneByte)
{
    PORTB = oneByte;
    _delay_ms(2);
}

void loadShiftRegister(int regClk, int latchClk, int dataPin, uint8_t data)
{
    uint8_t mask;
    for (int i=7; i>=0; i--)
    {
        OUT_PORT &= ~(1 << regClk);//Triggers on rising edge of clock, must bring low first

        mask = 1 << i;
        if (mask & data)
            OUT_PORT |= (1 << dataPin);
        else
            OUT_PORT &= ~(1 << dataPin);

        OUT_PORT |= (1 << regClk);//Loads data from data pin into shift reg on rising edge
    }
    OUT_PORT &= ~(1 << latchClk);
    OUT_PORT |= (1 << latchClk);
}
void loadShiftRegister_(int regClk, int latchClk, int dataPin, uint8_t* data, int numBits) {
    if (numBits <= 0)
        return;

    int N = (numBits-1)/8 + 1;
    for (int i=numBits-1; i>=0; i--)
    {
        OUT_PORT &= ~(1 << regClk);
        
        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        uint8_t mask = 1 << bitSel;
        if (mask & data[idx])
            OUT_PORT |= (1 << dataPin);
        else
            OUT_PORT &= ~(1 << dataPin);

        OUT_PORT |= (1 << regClk);
    }
    OUT_PORT &= ~(1 << latchClk);
    OUT_PORT |= (1 << latchClk);
}

volatile uint8_t buffer[BUF_SZ][BUF_SZ] = {
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0}
                           };
volatile uint8_t bufferBits[2] = {0x00, 0x00};

//consider returning the buffer bits array instead
void bufferToBits()
{
    int i, j;
    uint8_t temp[2] = {0x00, 0x00};
    for (i=0; i<BUF_SZ; i++) 
    {
        for (j=0; j<BUF_SZ; j++)
        {
            if (buffer[i][j])
            {
                if (i < 2)
                {
                    int offset = BUF_SZ*i + j;
                    temp[i/2] |= (0x80 >> offset);
                }
            
                else
                {
                
                    int i_ = i-2;
                    int offset = BUF_SZ*i_ + j;
                    temp[i/2] |= (0x80 >> offset);
                }
            }
        }
    } 
    for (int i=0; i<2; i++)
        bufferBits[i] = temp[i];   
}
void clearBuffer()
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            buffer[i][j] = 0;
}
void drawBuffer()
{
    
    bufferToBits();
    uint8_t copy[2];
    for (int i=0; i<2; i++)
        copy[i] = bufferBits[i];
    loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, copy, 16);
}

int main(void) 
{
    //-----------INITS------------//
    OUT_DDR |= 0xE0;
    uint8_t data[4] = {0xF0, 0x00, 0xF0, 0x0F};
    uint8_t data_alt[4][2] = {
                                {0xFF, 0xFF},
                                {0x00, 0x00},
                                {0xA5, 0x5A},
                                {0x80, 0x01}
                            };


    uint8_t cnt = 0;
    //-------EVENT LOOP-----------//
    while(1) 
    {
        //loadShiftRegister(REG_CLK, LATCH_CLK, DATA_PIN, data[cnt]);
        //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, data_alt[cnt], 16);
        //cnt++;
        //if (cnt == 4)
        //    cnt = 0;
        clearBuffer();
        drawBuffer();
        _delay_ms(100);

        for (int i=0; i<BUF_SZ; i++)
        {
            for (int j=0; j<BUF_SZ; j++)
            {
                buffer[i][j] = 1;
                drawBuffer();
                _delay_ms(100);
            }
        }
        //drawBuffer();
        //_delay_ms(500);
        //_delay_ms(1000);
    }
    return(0);

}
