#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "FrameBuffer.h"
#include "LedDriver.h"

#define OUT_DDR DDRC
#define OUT_PORT PORTC
#define REG_CLK PC4
#define LATCH_CLK PC3
#define DATA_PIN PC5
#define BUF_SZ 4
#define TIMER_DELAY 21


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
//Old function, want to reverse order of load
void loadShiftRegister_old(int regClk, int latchClk, int dataPin, volatile uint8_t* data, int numBits) {
    if (numBits <= 0)
        return;
    int i, numBits_ = numBits - 1;
    int N = (numBits_)/8 + 1;
    for (i=numBits_; i>=0; i--)
    {
        OUT_PORT &= ~(1 << regClk);
        
        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & data[idx])
            OUT_PORT |= (1 << dataPin);
        else
            OUT_PORT &= ~(1 << dataPin);

        OUT_PORT |= (1 << regClk);
    }
    OUT_PORT &= ~(1 << latchClk);
    OUT_PORT |= (1 << latchClk);
}
void loadShiftRegister_(int regClk, int latchClk, int dataPin, volatile uint8_t* data, int numBits) {
    if (numBits <= 0)
        return;
    int i, numBits_ = numBits - 1;
    int N = (numBits_)/8 + 1;
    for (i=0; i<numBits; i++)
    {
        OUT_PORT &= ~(1 << regClk);

        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & data[idx])
            OUT_PORT |= (1 << dataPin);
        else
            OUT_PORT &= ~(1 << dataPin);

        OUT_PORT |= (1 << regClk);
    }
    OUT_PORT &= ~(1 << latchClk);
    OUT_PORT |= (1 << latchClk);
}



volatile uint8_t buffer[BUF_SZ][BUF_SZ][BUF_SZ] = {
                        {
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0}
                        },
                        {
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0}
                        },
                        {
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0}
                        },
                        {
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0}
                        }
                    };

volatile FrameBuffer framebuffer;
volatile LedDriver ledDriver;
volatile uint8_t bufferBits[BUF_SZ][2] = {
                                        {0x00, 0x00},
                                        {0x00, 0x00},
                                        {0x00, 0x00},
                                        {0x00, 0x00}
                                    };
volatile uint8_t cnt = 0;
//consider returning the buffer bits array instead
void bufferToBits()
{
    int k, i, j;
    uint8_t temp[4][2] = {
                            {0x00, 0x00},
                            {0x00, 0x00},
                            {0x00, 0x00},
                            {0x00, 0x00}
                        };
    for (k=0; k<BUF_SZ; k++) {
        for (i=0; i<BUF_SZ; i++) 
        {
            for (j=0; j<BUF_SZ; j++)
            {
                if (buffer[k][i][j])
                {
                    if (i < 2)
                    {
                        int offset = BUF_SZ*i + j;
                        temp[k][i/2] |= (0x80 >> offset);
                    }
            
                    else
                    {
                
                        int i_ = i-2;
                        int offset = BUF_SZ*i_ + j;
                        temp[k][i/2] |= (0x80 >> offset);
                    }
                }
            }
        }
    } 
    for (k=0; k<BUF_SZ; k++)
    {
        for (i=0; i<2; i++)
            bufferBits[k][i] = temp[k][i];   
    }
}
void bufferToBitsQuick()
{
    int k, i, j, offset, i_;
    for (k=0; k<BUF_SZ; k++)
    {
        for (i=0; i<BUF_SZ; i++)
        {
            for (j=0; j<BUF_SZ; j++)
            {
                if (buffer[k][i][j])
                {
                    if (i < 2)
                    {
                        offset = BUF_SZ*i + j;
                        bufferBits[k][i/2] |= (0x80 >> offset);
                    }

                    else
                    {

                        i_ = i-2;
                        offset = BUF_SZ*i_ + j;
                        bufferBits[k][i/2] |= (0x80 >> offset);
                    }
                }
            }
        }
    }
}

void clearBuffer()
{
    for (int k=0; k<BUF_SZ; k++)
        for (int i=0; i<BUF_SZ; i++)
            for (int j=0; j<BUF_SZ; j++)
                buffer[k][i][j] = 0;
}
void drawBuffer()
{    
    bufferToBits();
    uint8_t copy[2];
    for (int i=0; i<2; i++)
        copy[i] = bufferBits[cnt][i];
    loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, copy, 16);
}
void drawBufferQuick()
{
    bufferToBitsQuick();
    loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, bufferBits[cnt], 16);
}

void initTimer()
{
    //init timer 1
    cli();              //disable global interrupts
    TCCR0A = 0;         //Clear TTR1 registers
    TCCR0B = 0;

    //Enable timer1 overflow interrupt
    //TIMSK0 = (1 << TOIE0);
    TIMSK0 = (1 << OCIE0A);

    
    //TCCR0B |= ((1 << CS01) | (0 << CS00));// 1/8 clock speed
    //TCCR0B |= ((1 << CS02) | (0 << CS01) | (1 << CS00));// 1/1024
    //TCCR0B |= ((1 << CS01) | (1 << CS00));// 1/64 clock speed
    TCCR0B |= ((1 << CS02) | (0 << CS01) | (0 << CS00));// 1/256 clock speed    

    // turn on CTC mode:
    TCCR0A |= (1 << WGM01);

    //TCCR1B |= (1 << CS10); //No Scaling 1 MHz
    //TCCR1B |= (1 << CS11); // 1/8 clock speed
    //uint16_t val = 60;
    //OCR1A = val;
    OCR0A = TIMER_DELAY;//21 is a good value
    TCNT0 = 0;   
    /*
    TCCR0B |= ((1 << CS01) | (1 << CS00));// 1/64 clock speed
    OCR0A = 1;
    */

    //Set 2nd timer
    //TCCR1B |= (1 << CS11)|(0 << CS10);// Set at 1/8th clock speed
    

    sei();
}

ISR(TIMER0_COMPA_vect)
{
    //bufferBits[0] ^= 0xFF;
    //bufferBits[1] ^= 0xFF;
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, bufferBits, 16);
    //for (int i=0; i<4; i++)
    //{
    //    buffer[3-i][i] = 1;
    //}
    //bufferToBitsQuick();
    //PORTD = 0;
    GND_PORTS = 0;
    //TCNT1 = 0;


    
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, bufferBits[cnt], 16);
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, framebuffer.bufferBits[cnt], 16);
    loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, framebuffer.bufferBits[framebuffer.currLevel], 16);
    //void loadShiftRegister_(int regClk, int latchClk, int dataPin, volatile uint8_t* data, int numBits) {
    /*
    int i, numBits_ = 16 - 1;
    int N = (numBits_)/8 + 1;
    for (i=numBits_; i>=0; i--)
    {
        OUT_PORT &= ~(1 << REG_CLK);

        int idx = N - 1 - i/8;
        int bitSel = i - (N - 1 - idx)*8;
        //uint8_t mask = 1 << bitSel;
        //if (mask & data[idx])
        if ((1 << bitSel) & bufferBits[cnt][idx])
            OUT_PORT |= (1 << DATA_PIN);
        else
            OUT_PORT &= ~(1 << DATA_PIN);

        OUT_PORT |= (1 << REG_CLK);
    }
    OUT_PORT &= ~(1 << LATCH_CLK);
    OUT_PORT |= (1 << LATCH_CLK);
    */
    //uint16_t timer_val = TCNT1;
    //uint8_t time[2] = {timer_val >> 8, timer_val};
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, time, 16); 

    //PORTB ^= 0x01;
    //PORTD = 1 << cnt;
    GND_PORTS = 1 << framebuffer.currLevel;

    /*
    cnt++;
    if (cnt == 4)
        cnt = 0;
    */
    framebuffer.currLevel++;
    if (framebuffer.currLevel == 4)
        framebuffer.currLevel = 0;
    
}

/*
ISR(TIMER0_OVF_vect) {
    //bufferBits[0] ^= 0xFF;
    //bufferBits[1] ^= 0xFF;
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, bufferBits, 16);
    drawBufferQuick();
    PORTB ^= 0x01;
}
*/
int main(void) 
{
    //-----------INITS------------//
    OUT_DDR |= 0xFF;
    initFrameBuffer(&framebuffer);
    setLedDriver(&ledDriver, REG_CLK, LATCH_CLK, DATA_PIN);

    uint8_t data[4] = {0xF0, 0x00, 0xF0, 0x0F};
    uint8_t data_alt[4][2] = {
                                {0xFF, 0xFF},
                                {0x00, 0x00},
                                {0xA5, 0x5A},
                                {0x80, 0x01}
                            };
    DDRD |= 0x0F;
    PORTD = 0x00;

    uint8_t cnt = 0;
    int delay = 50;
    initTimer();

    clearBuffer();
    drawBuffer();
    testFunc();
    //-------EVENT LOOP-----------//
    while(1) 
    {
        //loadShiftRegister(REG_CLK, LATCH_CLK, DATA_PIN, data[cnt]);
        //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, data_alt[cnt], 16);
        //cnt++;
        //if (cnt == 4)
        //    cnt = 0;
        clearBuffer();
        bufferToBits();

        clearFrameBuffer(&framebuffer);
        frameBufferToBits(&framebuffer);
        //drawBuffer();
        _delay_ms(delay);
        
        for (int k=0; k<BUF_SZ; k++)
        {
            for (int i=0; i<BUF_SZ; i++)
            {
                for (int j=0; j<BUF_SZ; j++)
                {
                    buffer[k][i][j] = 1;
                    bufferToBits();

                    setPixel(&framebuffer, i, j, k);
                    frameBufferToBits(&framebuffer);
                    //drawBuffer();
                    _delay_ms(delay);
                }
            }
        }
        //drawBuffer();
        //_delay_ms(500);
        //_delay_ms(1000);
    }
    return(0);

}
