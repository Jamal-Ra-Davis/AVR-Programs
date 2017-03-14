#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define SR_DDR DDRB
#define SR_PORT PORTB
#define DATA_PIN PB0
#define REG_CLK PB1
#define LATCH_CLK PB2

#define SPI_CLK PB5
#define SPI_LATCH PB2
#define SPI_DATA PB3

#define FBUF_SZ 2
#define FBUF_REGS 2

#define TIMER_DELAY 4
#define BIT_COUNTER 6

//                          0        15   
volatile uint8_t data[2] = {0x04, 0x00};
volatile uint8_t data2[2] = {0x00, 0x00};
volatile uint8_t bit_counter = 0;

typedef struct {
    int currLevel;
    
    uint8_t bufferBits[FBUF_SZ][FBUF_REGS];//16 bits: Red(0:3), Green(4:7), Blue(8:11), NC(12:15)
    uint8_t bufferBits_alt[FBUF_SZ][BIT_COUNTER][FBUF_REGS];
    uint8_t buffer[FBUF_SZ][FBUF_SZ][FBUF_SZ][3];
} FrameBuffer;

volatile FrameBuffer framebuffer;

void clearFrameBuffer(FrameBuffer *framebuffer)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                for (int c=0; c<3; c++)
                {
                    framebuffer->buffer[k][i][j][c] = 0;
                }
            }
        }
    }
}
void setFrameBuffer(FrameBuffer *framebuffer)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                for (int c=0; c<3; c++)
                {
                    framebuffer->buffer[k][i][j][c] = 1;
                }
            }
        }
    }
}
void setFrameBufferColor(FrameBuffer *framebuffer, uint8_t r, uint8_t g, uint8_t b)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                    framebuffer->buffer[k][i][j][0] = r;
                    framebuffer->buffer[k][i][j][1] = g;
                    framebuffer->buffer[k][i][j][2] = b;
            }
        }
    }
}
void frameBufferToBits_DB_NEW(FrameBuffer* framebuffer)
{
    uint8_t temp[FBUF_SZ][FBUF_REGS];
    int reg_idx, offset;

    for (int k=0; k<FBUF_SZ; k++)
    {
        reg_idx = 0;
        offset = 7;
        if (offset == -1)
            offset = 7;

        for (int i=0; i<FBUF_REGS; i++)
            temp[k][i] = 0x00;

        for (int c=0; c<3; c++)
        {
            for (int i=0; i<FBUF_SZ; i++)
            {
                for (int j=0; j<FBUF_SZ; j++)
                {
                    if (framebuffer->buffer[k][i][j][c])
                        temp[k][reg_idx] |= (1 << offset);
                    else
                        temp[k][reg_idx] &= ~(1 << offset);

                    offset--;
                    if (offset < 0)
                    {
                        offset = 7;
                        reg_idx++;
                    }        
                }
            }
        }
    }

    for (int i=0; i<FBUF_SZ; i++)
    {
        for (int j=0; j<FBUF_REGS; j++)
        {
            framebuffer->bufferBits[i][j] = temp[i][j];
        }
    }
}
void frameBufferToBits_DB_alt(FrameBuffer* framebuffer)
{
    uint8_t temp[FBUF_SZ][BIT_COUNTER][FBUF_REGS];
    int reg_idx, offset;

    for (int k=0; k<FBUF_SZ; k++)
    {
        reg_idx = 0;
        offset = 7;
        if (offset == -1)
            offset = 7;

        for (int i=0; i<FBUF_REGS; i++)
            for (int b=0; b<BIT_COUNTER; b++)
                temp[k][b][i] = 0x00;

        for (int c=0; c<3; c++)
        {
            for (int i=0; i<FBUF_SZ; i++)
            {
                for (int j=0; j<FBUF_SZ; j++)
                {
                    if (framebuffer->buffer[k][i][j][c])
                    {
                        for (int b=0; b<framebuffer->buffer[k][i][j][c]; b++)
                            temp[k][b][reg_idx] |= (1 << offset);
                    }

                    offset--;
                    if (offset < 0)
                    {
                        offset = 7;
                        reg_idx++;
                    }
                }
            }
        }
    }

    for (int i=0; i<FBUF_SZ; i++)
    {
        for (int j=0; j<FBUF_REGS; j++)
        {
            for (int b=0; b<BIT_COUNTER; b++)
            {
                framebuffer->bufferBits_alt[i][b][j] = temp[i][b][j];
            }
        }
    }
}



uint8_t spi_tranceiver(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}
void sr_loadData_spi(volatile uint8_t* array, int N)
{ 
    SR_PORT &= ~(1 << SPI_LATCH);
    for (int i=N-1; i>=0; i--)
        spi_tranceiver(array[i]);
    SR_PORT |= (1 << SPI_LATCH);
}
void sr_loadData_spi_fast(volatile uint8_t* array, int N)
{
    SR_PORT &= ~(1 << SPI_LATCH);
    for (int i=N-1; i>=0; i--)
    {
        SPDR = array[i];
        while (!(SPSR & (1 << SPIF)));
    }
    SR_PORT |= (1 << SPI_LATCH);
}

void sr_loadData_alt(volatile uint8_t* data, int numBits)
{
    uint8_t regClk = REG_CLK;
    uint8_t dataPin = DATA_PIN;
    uint8_t latchClk = LATCH_CLK;

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
    SR_PORT &= ~(1 << latchClk);
    SR_PORT |= (1 << latchClk);
}

void spi_init_master()
{
    DDRB |= (1 << SPI_DATA)|(1 << SPI_CLK)|(1 << SPI_LATCH);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << DORD);
    SPSR = (1 << SPI2X);

    //SR_PORT &= ~((1 << SPI_CLK)|(1 << SPI_DATA)|(1 << SPI_LATCH));
}
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
    PORTB &= ~((1 << PB0) | (1 << PB1));
    //uint8_t big_buf[5] = {0x00, 0x00, 0x00, framebuffer.bufferBits[framebuffer.currLevel][1], framebuffer.bufferBits[framebuffer.currLevel][0]};
    //uint8_t big_buf[10] = {framebuffer.bufferBits[framebuffer.currLevel][0], framebuffer.bufferBits[framebuffer.currLevel][1], 0x00, 0x00, 0x00,
    //                        0x00, 0x00, 0x00, 0x00, 0x00};



    //sr_loadData_alt(&ledDriver, big_buf, 80);
    //sr_loadData_spi(framebuffer.bufferBits[framebuffer.currLevel], FBUF_REGS);
    sr_loadData_spi(framebuffer.bufferBits_alt[framebuffer.currLevel][bit_counter], FBUF_REGS);
    //sr_loadByte(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel][1]);
    //sr_loadByte(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel][0]);


    //void loadShiftRegister_(int regClk, int latchClk, int dataPin, volatile uint8_t* data, int numBits) {
    //uint8_t time[2] = {timer_val >> 8, timer_val};
    //loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, time, 16); 

    PORTB |= 1 << framebuffer.currLevel;

    bit_counter++;
    if (bit_counter == BIT_COUNTER)
    {
        bit_counter = 0;
        framebuffer.currLevel++;
    }

    if (framebuffer.currLevel == 2)
        framebuffer.currLevel = 0;
}


int main(void) 
{
    DDRB |= (1 << PB0) | (1 << PB1);
    PORTB &= ~((1 << PB0) | (1 << PB1));

    clearFrameBuffer(&framebuffer);
    framebuffer.currLevel = 0;

    spi_init_master();
    initTimer(); 
    //SR_DDR |= (1 << DATA_PIN)|(1 << REG_CLK)|(1 << LATCH_CLK);
    //SR_PORT = 0x00;    

    //sr_loadData_spi(data2);    
    //SR_PORT &= ~(1 << SPI_LATCH);

    //-----------INITS------------//

    int r= BIT_COUNTER/2;
    int g= BIT_COUNTER/2;
    int b= BIT_COUNTER/2;

    //-------EVENT LOOP-----------//
    //SR_PORT &= ~(1 << SPI_LATCH);
    while(1) 
    {
//        sr_loadData_spi(data, 2);
//        _delay_ms(200);

/*
        clearFrameBuffer(&framebuffer);
        for (int k=0; k<FBUF_SZ; k++)
        {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                for (int c=0; c<3; c++) 
                {
                    //clearFrameBuffer(&framebuffer);
                    framebuffer.buffer[k][i][j][c] = k+1;
                    frameBufferToBits_DB_alt(&framebuffer);
                    //sr_loadData_spi(framebuffer.bufferBits[0], FBUF_REGS);
                    _delay_ms(200);            
                }
            }
        }
        }
  */      
/*
        for (int r=0; r<5; r++)
        {
            for (int g=0; g<5; g++)
            {
                for (int b=0; b<5; b++)
                {
                    clearFrameBuffer(&framebuffer);
                    setFrameBufferColor(&framebuffer, r, g, b);
                    frameBufferToBits_DB_alt(&framebuffer);
                    _delay_ms(200);
                }
            }
        }
*/
        int dir = rand()%3;

        if (dir == 0)
        {
        	if (rand()%2) 
            	r++;
        	else
            	r--;
        }
        else if (dir == 1)
        {
        	if (rand()%2)    
            	g++;
        	else
            	g--;
        }
        else if (dir == 2)
        {
        	if (rand()%2)    
            	b++;
       		else
            	b--;
        }


		if (r < 0)
            r = 0;
        if (r > BIT_COUNTER)
            r = BIT_COUNTER;
		if (g < 0)
            g = 0;
        if (g > BIT_COUNTER)
            g = BIT_COUNTER;
		if (b < 0)
            b = 0;
        if (b > BIT_COUNTER)
            b = BIT_COUNTER;
	

        clearFrameBuffer(&framebuffer);
        setFrameBufferColor(&framebuffer, r, g, b);
        frameBufferToBits_DB_alt(&framebuffer);
        _delay_ms(50);
    


/*
        for (int l=0; l<5; l++)
        {
        clearFrameBuffer(&framebuffer);
        for (int k=0; k<FBUF_SZ; k++)
        {
            for (int i=0; i<FBUF_SZ; i++)
            {
                for (int j=0; j<FBUF_SZ; j++)
                {
                    for (int c=0; c<3; c++)
                        framebuffer.buffer[k][i][j][c] = rand()%5;
                }
            }
        }
        frameBufferToBits_DB_alt(&framebuffer);
        _delay_ms(200);
        }
*/


        //frameBufferToBits_DB_NEW(&framebuffer);
        //_delay_ms(200);
        

/*
        clearFrameBuffer(&framebuffer);
        framebuffer.buffer[0][0][0][0] = 1;
        frameBufferToBits_DB_NEW(&framebuffer);
        sr_loadData_spi(framebuffer.bufferBits[0], FBUF_REGS);
        _delay_ms(500);

        clearFrameBuffer(&framebuffer);
        framebuffer.buffer[0][1][1][0] = 1;
        frameBufferToBits_DB_NEW(&framebuffer);
        sr_loadData_spi(framebuffer.bufferBits[0], FBUF_REGS);
        _delay_ms(500);
*/



//        for (int i=0; i<8; i++)
//        {
//            data2[0] = (0x01 << i);
//            sr_loadData_spi(data2, 2);
//            _delay_ms(500);
//        }


        //spi_tranceiver(0x81);
        //SPDR = 0x01;
        //while (!(SPSR & (1 << SPIF)));
//        data[0] = 0xAA;
//        data[1] = 0xF1;
//        sr_loadData_alt(data, 16);
//        _delay_ms(1000);
//        data[0] = 0x00;
//        data[1] = 0x00;
//        sr_loadData_alt(data, 16);
//        _delay_ms(1000);



/*
        for (int i=1; i<=5; i++)
        {
            data[0] = 0xFF;
            data[1] = 0xFF;
            sr_loadData_alt(data, 16);
            for (int j=0; j<i; j++)
                _delay_ms(100);
            data[0] = 0x00;
            data[1] = 0x00;
            sr_loadData_alt(data, 16);
            for (int j=0; j<i; j++)
                _delay_ms(100);
        }

*/
/*
        int i, j;
        for (i=0; i<256; i++)
        {
            for (j=0; j<16; j++)
            {
                data[0] = i;
                data[1] = (j << 4);
                sr_loadData_alt(data, 16);
                _delay_ms(100);
            }
        }
        data[0] = 0xAA;
        data[1] = 0xAA;
        sr_loadData_alt(data, 16);
        _delay_ms(1000);
*/
/*
        //SR_PORT |= (1 << DATA_PIN);
        data[0] = 0x00;
        data[1] = 0x00;
        sr_loadData_alt(data, 16);
        _delay_ms(1000);
        //SR_PORT &= ~(1 << DATA_PIN);
        data[0] = 0xFF;
        data[1] = 0xFF;
        sr_loadData_alt(data, 16);
        _delay_ms(1000);
*/
        
        //sr_loadData_alt(data, 12);
        //sr_loadData_spi(data2);
        //_delay_ms(1);


    }
    return(0);

}
