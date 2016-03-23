#include <avr/power.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define GSCLK_DDR DDRB
#define GSCLK_PORT PORTB
#define GSCLK_PIN PB0

#define SIN_DDR DDRB
#define SIN_PORT PORTB
#define SIN_PIN PB3

#define SCLK_DDR DDRB
#define SCLK_PORT PORTB
#define SCLK_PIN PB5

#define BLANK_DDR DDRB
#define BLANK_PORT PORTB
#define BLANK_PIN PB2

#define DCPRG_DDR DDRD
#define DCPRG_PORT PORTD
#define DCPRG_PIN PD4

#define VPRG_DDR DDRD
#define VPRG_PORT PORTD
#define VPRG_PIN PD7

#define XLAT_DDR DDRB
#define XLAT_PORT PORTB
#define XLAT_PIN PB1

#define pulse(port, pin) do {\
                                port |= (1 << pin);\
                                port &= ~(1 << pin);\
                        } while (0)


#define TLC5940_N 1
#define NUM_CHANNELS  (16*TLC5940_N)
/*
uint8_t dcData[96*TLC5940_N] = {
    0, 0, 0, 0, 1, 1,   //Ch 15
    0, 0, 0, 0, 1, 1,   //Ch 14
    0, 0, 0, 0, 1, 1,   //Ch 13
    0, 0, 0, 0, 1, 1,   //Ch 12
    0, 0, 0, 0, 1, 1,   //Ch 11
    0, 0, 0, 0, 1, 1,   //Ch 10
    0, 0, 0, 0, 1, 1,   //Ch 9
    0, 0, 0, 0, 1, 1,   //Ch 8
    0, 0, 0, 0, 1, 1,   //Ch 7
    0, 0, 0, 0, 1, 1,   //Ch 6
    0, 0, 0, 0, 1, 1,   //Ch 5
    0, 0, 0, 0, 1, 1,   //Ch 4
    0, 0, 0, 0, 1, 1,   //Ch 3
    0, 0, 0, 0, 1, 1,   //Ch 2
    0, 0, 0, 0, 1, 1,   //Ch 1
    0, 0, 0, 0, 1, 1    //Ch 0
};
*/

uint8_t dcData[12*TLC5940_N] = {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
};

/*
uint8_t gsData[192*TLC5940_N] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 14
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,     //Ch 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,     //Ch 11
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,     //Ch 10
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,     //Ch 9 
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,     //Ch 8
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,     //Ch 7
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,     //Ch 6
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,     //Ch 5
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 4
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 3
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 2
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //Ch 1
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1      //Ch 0
};
*/

uint8_t gsData[24*TLC5940_N] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000001,
    0b00000000,
    0b00100000,
    0b00000100,
    0b00000000,
    0b10000000,
    0b00010000,
    0b00000010,
    0b00000000,
    0b01000000,
    0b00001000,
    0b00000001,
    0b00000000,
    0b00100000,
    0b00000100,
    0b00000000,
    0b10000000,
    0b00001111,
    0b11111111
};
uint8_t gsData2[24*TLC5940_N] = {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
};


void TLC5940_SetGS(int channel, uint16_t value)
{
    channel = NUM_CHANNELS - 1 - channel;
    int i = (channel*3)/2;

    switch (channel % 2)
    {
        case 0:
        {
            gsData[i] = (value >> 4);
            i++;
            gsData[i] = (0x0F & gsData[i]) | (uint8_t)(value << 4);
            break;
        }
        default:
        {
            gsData[i] = (0xF0 & gsData[i]) | ((uint8_t)(value >> 8) & 0x0F);
            i++;
            gsData[i] = (uint8_t)value;
            break;
        }
    }
}


void resetGSData()
{
//    for (int i=0; i<192*TLC5940_N; i++)
//        gsData[i] = 0;
    for (int i=0; i<24*TLC5940_N; i++)
        gsData[i] = 0x00;
}
void setGSData()
{
//    for (int i=0; i<192*TLC5940_N; i++)
//        gsData[i] = 1;
    for (int i=0; i<24*TLC5940_N; i++)
        gsData[i] = 0xFF;
}


ISR(TIMER0_COMPA_vect)
{
/*
    uint8_t firstCycleFlag = 0;
    static uint8_t xlatNeedsPulse = 0;

    BLANK_PORT |= (1 << BLANK_PIN);
    
    if (VPRG_PORT & (1 << VPRG_PIN))
    {
        VPRG_PORT &= ~(1 << VPRG_PIN);
        firstCycleFlag = 1;
    } 

    if (xlatNeedsPulse)
    {
        pulse(XLAT_PORT, XLAT_PIN);
        xlatNeedsPulse = 0;
    }

    if (firstCycleFlag)
        pulse(SCLK_PORT, SCLK_PIN);
*/
    static uint8_t group = 0;

    static uint8_t xlatNeedsPulse = 0;
    BLANK_PORT |= (1 << BLANK_PIN);
    if (VPRG_PORT & (1 << VPRG_PIN))
    {
        VPRG_PORT &= ~(1 << VPRG_PIN);
        if (xlatNeedsPulse)
        {
            pulse(XLAT_PORT, XLAT_PIN);
            xlatNeedsPulse = 0;
        }
        pulse(SCLK_PORT, SCLK_PIN);
    }
    else if (xlatNeedsPulse)
    {
        PORTC = 0x00;

        pulse(XLAT_PORT, XLAT_PIN);
        xlatNeedsPulse = 0;

        if (group == 1)
            PORTC = 0b00100000;
        else
            PORTC = 0b00010000;
    }

    BLANK_PORT &= ~(1 << BLANK_PIN);

    // Below this we have 4096 cycles to shift in the data for the next cycle
    /*
    uint8_t dataCounter = 0;
    while (1)
    {
        if (!(dataCounter > TLC5940_N*192 - 1))
        {
            if (gsData[dataCounter])
                SIN_PORT |= (1 << SIN_PIN);    
            else
                SIN_PORT &= ~(1 << SIN_PIN);
            pulse(SCLK_PORT, SCLK_PIN);
            dataCounter++;
        }
        else
        {
            xlatNeedsPulse = 1;
            break;
        }
    }
    */
    for (int i=0; i<24*TLC5940_N; i++)
    {
        if (group == 0)
            SPDR = gsData[i];
        else
            SPDR = gsData2[i];
        while (!(SPSR & (1 << SPIF)));
    }
    xlatNeedsPulse = 1;
    
    group++;
    if (group == 2)
        group = 0;
}

void TLC5940_Init()
{
    GSCLK_DDR |= (1 << GSCLK_PIN);
    SCLK_DDR |= (1 << SCLK_PIN);
    DCPRG_DDR |= (1 << DCPRG_PIN);
    VPRG_DDR |= (1 << VPRG_PIN);
    XLAT_DDR |= (1 << XLAT_PIN);
    BLANK_DDR |= (1 << BLANK_PIN);    
    SIN_DDR |= (1 << SIN_PIN);

    GSCLK_PORT &= ~(1 << GSCLK_PIN);
    SCLK_PORT &= ~(1 << SCLK_PIN);  
    DCPRG_PORT &= ~(1 << DCPRG_PIN);
    VPRG_PORT |= (1 << VPRG_PIN); 
    XLAT_PORT &= ~(1 << XLAT_PIN);
    BLANK_PORT |= (1 << BLANK_PIN);


    // CTC with OCR0A as TOP
    TCCR0A = (1 << WGM01);
    
    // clk_io/1024 (From prescaler)
    TCCR0B = ((1 << CS02) | (1 << CS00));

    // Generate an interrupt every 4096 clock cycles
    OCR0A = 3;

    // Enable Timer/Counter0 Compare Match A interrupt
    TIMSK0 |= (1 << OCIE0A);


    // Enable SPI, Master, set clock rate fck/2
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR = (1 << SPI2X);
}

void TLC5940_ClockInDC()
{
    DCPRG_PORT |= (1 << DCPRG_PIN);
    VPRG_PORT |= (1 << VPRG_PIN);
/*
    uint8_t counter = 0;
    while(1)
    {
        if (counter > TLC5940_N*96 - 1)
        {
            pulse(XLAT_PORT, XLAT_PIN);
            break;
        }
        else
        {
            if (dcData[counter])
                SIN_PORT |= (1 << SIN_PIN);
            else
                SIN_PORT &= ~(1 << SIN_PIN);

            pulse(SCLK_PORT, SCLK_PIN);
            counter++;
        }
    }
*/
    for (int i=0; i<12*TLC5940_N; i++)
    {
        SPDR = dcData[i];
        while (!(SPSR & (1 << SPIF)));
    }
    pulse(XLAT_PORT, XLAT_PIN);
}

void TLC5940_SetGS_And_GS_PWM()
{
    uint8_t firstCycleFlag = 0;
    if (VPRG_PORT & (1 << VPRG_PIN))
    {
        VPRG_PORT &= ~(1 << VPRG_PIN);
        firstCycleFlag = 1;
    }

    uint16_t GSCLK_Counter = 0;
    uint8_t Data_Counter = 0;
    
    BLANK_PORT &= ~(1 << BLANK_PIN);

    while (1)
    {
        if (GSCLK_Counter > 4095)
        {
            BLANK_PORT |= (1 << BLANK_PIN);
            pulse(XLAT_PORT, XLAT_PIN);
            if (firstCycleFlag)
                pulse(SCLK_PORT, SCLK_PIN);
            firstCycleFlag = 0;
            break;
        }
        else
        {
            if (!(Data_Counter > 192*TLC5940_N - 1))
            {
                if (gsData[Data_Counter])
                    SIN_PORT |= (1 << SIN_PIN);
                else
                    SIN_PORT &= ~(1 << SIN_PIN);

                pulse(SCLK_PORT, SCLK_PIN);
                Data_Counter++;
            }
            pulse(GSCLK_PORT, GSCLK_PIN);
            GSCLK_Counter++;
        }
    }
}

int main(void) 
{
    DDRC = 0xFF;
    PORTC = 0x00;


    //Run at 8 MHz
    clock_prescale_set(clock_div_1);

    //-----------INITS------------//
    //for (int i=0; i<92; i++)
    //    dcData[i]=1;

    TLC5940_Init();
    TLC5940_ClockInDC();
    
    //Enable interrupts
    sei();
     
    //uint16_t 
    //-------EVENT LOOP-----------//
    while(1) 
    {
/*
        resetGSData();
        for (int i=0; i<15; i=i+3)
        {
            TLC5940_SetGS(i, 0x00F);
            TLC5940_SetGS(i+1, 0x00F);
            TLC5940_SetGS(i+2, 0x00F);
            //TLC5940_SetGS(i, rand()%0xFFF);
            //TLC5940_SetGS(i+1, rand()%0xFFF);
            //TLC5940_SetGS(i+2, rand()%0xFFF);
        }
        _delay_ms(500);
*/
/*
        for (int i=0; i<24; i++)
        {
            gsData[i] = rand()%256;
        }
        _delay_ms(1000);
*/
        //_delay_ms(3000);
/*
        for (int j=0; j<3; j++)
        { 
            resetGSData();
            for (int i=j; i<15; i=i+3)
            {
                TLC5940_SetGS(i, (uint16_t)((i+1)*256));
                _delay_ms(500);
            }
        }
*/

        for (int i=0; i<15; i++)
        {
            resetGSData();
            TLC5940_SetGS(i, 0xFFF);
            _delay_ms(250);
        }
        for (int i=13; i>=1; i--)
        {
            resetGSData();
            TLC5940_SetGS(i, 0xFFF);
            _delay_ms(250);
        }
        //resetGSData();


/* 
        for (int i = 192*TLC5940_N-1; i>=0; i--)
        {
            gsData[i] = rand()%2;
        }
        _delay_ms(1000);
*/
//        for (int i=0; i<500; i++)
//        {
//            TLC5940_SetGS_And_GS_PWM();
//            _delay_ms(5);
//        }

    }
    return(0);

}
