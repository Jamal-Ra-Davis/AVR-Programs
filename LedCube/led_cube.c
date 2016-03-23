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
#define TIMER_DELAY 16 //60 Hz, delay 60
//#define TIMER_DELAY 8 //120 Hz, delay 30
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
    initTimer();
    
    uint8_t num = 0;

    //-------EVENT LOOP-----------//
    while(1) 
    {
        //clearFrameBuffer(&framebuffer);
        //frameBufferToBits(&framebuffer);
//        _delay_ms(DELAY_TIME);
       
        /* 
        for (int z=0; z<framebuffer.size; z++)
        {
            for (int x=0; x<framebuffer.size; x++)
            {
                for (int y=0; y<framebuffer.size; y++)
                {
                    setPixel(&framebuffer, x, y, z);
                    frameBufferToBits(&framebuffer);
                    _delay_ms(DELAY_TIME);
                }
            }
        }
        */
        /*
        for (int y=0; y<framebuffer.size; y++)
        {
            clearFrameBuffer(&framebuffer);
            for (int x=0; x<framebuffer.size; x++)
            {
                for (int z=0; z<framebuffer.size; z++)
                {
                    setPixel(&framebuffer, x, y, z);
                }
            }
            frameBufferToBits(&framebuffer);
            _delay_ms(DELAY_TIME);
        }
        for (int y=framebuffer.size-2; y>=1; y--)
        {
            clearFrameBuffer(&framebuffer);
            for (int x=0; x<framebuffer.size; x++)
            {
                for (int z=0; z<framebuffer.size; z++)
                {
                    setPixel(&framebuffer, x, y, z);
                }
            }
            frameBufferToBits(&framebuffer);
            _delay_ms(DELAY_TIME);
        }
        */
/*
        for (int i=0; i<20; i++)
        {
            shiftPlane_anm(&framebuffer, 0, DELAY_TIME);
            shiftPlane_anm(&framebuffer, 1, DELAY_TIME);
            shiftPlane_anm(&framebuffer, 2, DELAY_TIME);
        }
        
        for (int i=0; i<20; i++)
        {            
        for (int x=0; x<framebuffer.size; x++)
        {
            for (int y=0; y<framebuffer.size; y++)
            {
                clearFrameBuffer(&framebuffer);
                drawLine(&framebuffer, 0, x,  y);
                frameBufferToBits(&framebuffer);
                _delay_ms(DELAY_TIME);
            }
        }
        
        for (int x=0; x<framebuffer.size; x++)
        {
            for (int y=0; y<framebuffer.size; y++)
            {
                clearFrameBuffer(&framebuffer);
                drawLine(&framebuffer, 1, x,  y);
                frameBufferToBits(&framebuffer);
                _delay_ms(DELAY_TIME);
            }
        }
        for (int x=0; x<framebuffer.size; x++)
        {
            for (int y=0; y<framebuffer.size; y++)
            {
                clearFrameBuffer(&framebuffer);
                drawLine(&framebuffer, 2, x,  y);
                frameBufferToBits(&framebuffer);
                _delay_ms(DELAY_TIME);
            }
        }
        }
*/
        //clearFrameBuffer_(&framebuffer);
        //setFrameBuffer(&framebuffer);
        //frameBufferToBits(&framebuffer);
        //_delay_ms(DELAY_TIME);

/*
        for (int i=0; i<3; i++)
        {
            for (int j=0; j<20; j++)
                rotatePlane_anm(&framebuffer, i, DELAY_TIME);
        }
        for (int i=0; i<20; i++)
            snake_anm(&framebuffer, 2, DELAY_TIME);
*/  
      
        /*
        //if (!(PIND & (1 << buttonController.down.buttonPin)))
        if (getButtonStates(&buttonController))
        //if (buttonPressed(&buttonController, 'U'))
        {
            setFrameBuffer(&framebuffer);
            num = 0;
        }
        else
        {
            clearFrameBuffer_(&framebuffer);
            num = 1;
        }
        */
        //clearFrameBuffer_(&framebuffer);
        //uint8_t bNum[2] = {0xFF, 0xA5};
        //drawBinaryNumber(&framebuffer, timer_val, 2, 1);
        //drawBinaryNumber(&framebuffer, 0xB, 2, 1);
        //frameBufferToBits_DB(&framebuffer);
                
        //snake_anm(&framebuffer, 2, 2*DELAY_TIME);
        //_delay_ms(DELAY_TIME);
        //num++;
        //controlPixel(&framebuffer, DELAY_TIME);
        //snakeGame(&framebuffer, 3*DELAY_TIME);
        //tritrisGame(&framebuffer, 1*DELAY_TIME);
        //tallyScore_anm(&framebuffer, 167, 1, 2*DELAY_TIME);

        mainLoop(&framebuffer, DELAY_TIME);
    }
    return(0);

}
