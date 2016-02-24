#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "FrameBuffer.h"
#include "LedDriver.h"
#include "Vector3d.h"
#include "ButtonController.h"
#include "Tritris.h"

#define LEDS_DDR DDRC
#define GND_DDR DDRD
#define REG_CLK PC4
#define LATCH_CLK PC3
#define DATA_PIN PC5
//#define TIMER_DELAY 21 //45 Hz
//#define TIMER_DELAY 16 //60 Hz, delay 60
#define TIMER_DELAY 50 //120 Hz, delay 30
#define DELAY_TIME 45

volatile uint16_t timer_val;
volatile FrameBuffer framebuffer;
volatile LedDriver ledDriver;
ButtonController buttonController;

void initTimer()
{
    //init timers
    cli();              //disable global interrupts
    TCCR0A = 0;         //Clear TTR1 registers
    TCCR0B = 0;

    //Enable timer1 compareA interrupt
    TIMSK0 = (1 << OCIE0A);

    
    //TCCR0B |= ((1 << CS01) | (0 << CS00));// 1/8 clock speed
    //TCCR0B |= ((1 << CS02) | (0 << CS01) | (1 << CS00));// 1/1024
    //TCCR0B |= ((1 << CS01) | (1 << CS00));// 1/64 clock speed
    TCCR0B |= ((1 << CS02) | (0 << CS01) | (0 << CS00));// 1/256 clock speed    

    // turn on CTC mode:
    TCCR0A |= (1 << WGM01);

    OCR0A = TIMER_DELAY;//21 is a good value
    TCNT0 = 0;   

    //Set 2nd timer
    //TCCR1B |= (1 << CS11)|(0 << CS10);// Set at 1/8th clock speed
    //TCCR1B |= (1 << CS10); //No Scaling 1 MHz
    TCCR1B |= (1 << CS11); // 1/8 clock speed 

    sei();
}

ISR(TIMER0_COMPA_vect)
{
    GND_PORTS &= 0xF0;
    //uint8_t big_buf[5] = {0x00, 0x00, 0x00, framebuffer.bufferBits[framebuffer.currLevel][1], framebuffer.bufferBits[framebuffer.currLevel][0]};
    //uint8_t big_buf[10] = {framebuffer.bufferBits[framebuffer.currLevel][0], framebuffer.bufferBits[framebuffer.currLevel][1], 0x00, 0x00, 0x00,
    //                        0x00, 0x00, 0x00, 0x00, 0x00};
    TCNT1 = 0;//--


   
    //sr_loadData_alt(&ledDriver, big_buf, 80);
    sr_loadData_alt(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel], 16);
    
    //sr_loadByte(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel][1]);
    //sr_loadByte(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel][0]);
    

    //void loadShiftRegister_(int regClk, int latchClk, int dataPin, volatile uint8_t* data, int numBits) {
    timer_val = TCNT1;//--
    //uint8_t time[2] = {timer_val >> 8, timer_val};
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, time, 16); 

    GND_PORTS |= 1 << framebuffer.currLevel;

    framebuffer.currLevel++;
    if (framebuffer.currLevel == 4)
        framebuffer.currLevel = 0;
}

int main(void) 
{
    //-----------INITS------------//
    LEDS_DDR |= 0xFF;
    GND_DDR = 0x0F;
    GND_PORTS = 0x00;
    //PORTD |= (1 << PD7);


    initFrameBuffer(&framebuffer);
    setLedDriver(&ledDriver, REG_CLK, LATCH_CLK, DATA_PIN);
    initButtonController(&buttonController);

    setFrameBuffer(&framebuffer);
//    initTimer();
    
    uint8_t num = 0;
    uint8_t data[4][5] = {
                            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                            {0x00, 0x00, 0x00, 0x00, 0x00},
                            {0x00, 0x00, 0x00, 0x00, 0xFF},
                            {0x00, 0x00, 0x00, 0x00, 0x00}
                        };

    //-------EVENT LOOP-----------//
    while(1) 
    {
        for (int i=0; i<4; i++)
        {
            sr_loadData_alt(&ledDriver, data[i], 36);
            delay_ms(500);
        }
    }
    return(0);

}
