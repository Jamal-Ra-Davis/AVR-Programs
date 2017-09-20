#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "RingBuffer.h"
#include "text_data.h"

#define F_CPU 8000000UL

#define BAUD_RATE 9600
#define MYUBRR ((F_CPU/BAUD_RATE)/16 - 1)



#define FBUF_SZ 60
#define FBUF_HEIGHT 3
#define LED_PORT PORTB
#define LED_DDR	DDRB
#define TIMER_MAX 249

#define CLK_PIN (1 << PB5)
#define LAT_PIN (1 << PB2)
#define DATA_PIN (1 << PB3)

#define DEFAULT_INT_DELAY 68

#define RED 0
#define GREEN 1
#define BLUE 2


typedef struct
{
    uint8_t buffer[FBUF_SZ][8][3];
	int buf_idx;
} FrameBuffer;


int buf_idx;

uint32_t SYS_TIMER;
uint32_t time_delta;


FrameBuffer frame_buf;

/*
const uint8_t NUM_BUF[10][8] PROGMEM = {
							{0x00, 0x00, 0x3C, 0x42, 0x42, 0x3C, 0x00, 0x00},//0
							{0x00, 0x00, 0x00, 0x22, 0x7E, 0x02, 0x00, 0x00},//1
							{0x00, 0x00, 0x26, 0x4A, 0xA2, 0x22, 0x00, 0x00},//2
							{0x00, 0x00, 0x42, 0x52, 0x52, 0x2C, 0x00, 0x00},//3
							{0x00, 0x00, 0x78, 0x08, 0x7E, 0x08, 0x00, 0x00},//4
							{0x00, 0x00, 0x72, 0x52, 0x52, 0x4C, 0x00, 0x00},//5
							{0x00, 0x00, 0x3C, 0x52, 0x52, 0x4C, 0x00, 0x00},//6
							{0x00, 0x00, 0x40, 0x4E, 0x50, 0x60, 0x00, 0x00},//7
							{0x00, 0x00, 0x2C, 0x52, 0x52, 0x2C, 0x00, 0x00},//8
							{0x00, 0x00, 0x30, 0x48, 0x48, 0x3E, 0x00, 0x00} //9
						};
*/
void writeDigit(uint8_t digit, int index, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf);
void writeNumber(long number, int idx, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf);
uint8_t calcIntDelay(uint16_t period);
void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf);
void clearPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf);
void clearBuffer(FrameBuffer *buf);
void setBuffer(FrameBuffer *buf);

uint8_t serialDataAvailable()
{
    return (UCSR0A & (1<<RXC0));
}
unsigned char USART_Recieve()
{
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}
uint8_t usart_val=0;
ISR(USART_RX_vect)
{
    usart_val = UDR0;
}

uint8_t spi_tranceiver(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}


ISR(INT0_vect)//Hall-effect trigger
{
	uint32_t t_delta = SYS_TIMER - time_delta;
	time_delta = SYS_TIMER;
	if (t_delta >= 1000)
		OCR0A = DEFAULT_INT_DELAY;
	else if (t_delta >= 491)
		OCR0A = 255;
	else if (t_delta <= 32)
		OCR0A = 16;
	else
	{
		//OCR0A = calcIntDelay(delta);

		uint32_t val = 533*t_delta - 512;//== 533*t_delta + 512 - 1024
		OCR0A = (uint8_t)(val >> 10);
	}
	TCNT0 = 0;
	buf_idx = 0;
}
int w_idx, b_idx;
ISR(TIMER0_COMPA_vect)
{
/*
	w_idx = buf_idx + 20;
	if (w_idx >= FBUF_SZ)
		w_idx -= FBUF_SZ;

	b_idx = buf_idx + 40;
	if (b_idx >= FBUF_SZ)
		b_idx -= FBUF_SZ;


	LED_PORT &= ~LAT_PIN;
	spi_tranceiver(frame_buf.buffer[2][b_idx]);
	spi_tranceiver(frame_buf.buffer[1][w_idx]);
	spi_tranceiver(frame_buf.buffer[0][buf_idx]);
	LED_PORT |= LAT_PIN;
*/
	
	//Start frame
    spi_tranceiver(0x00);
    spi_tranceiver(0x00);
    spi_tranceiver(0x00);
    spi_tranceiver(0x00);


    //led frames
    for (int i=0; i<8; i++)
    {
        spi_tranceiver(0xFF);
        spi_tranceiver(frame_buf.buffer[buf_idx][i][BLUE]);
        spi_tranceiver(frame_buf.buffer[buf_idx][i][GREEN]);
        spi_tranceiver(frame_buf.buffer[buf_idx][i][RED]);
    }

    //end frame
    for (int i=0; i<8; i += 16)
    {
        spi_tranceiver(0xFF);
    }


	buf_idx++;
	if (buf_idx >= FBUF_SZ)
		buf_idx = 0;	
}
ISR(TIMER2_COMPA_vect)
{
	SYS_TIMER++;
}


int main(void) 
{
    //-----------INITS------------//
	clock_prescale_set(clock_div_1);

	LED_DDR = 0xFF;
	LED_PORT = 0x00;
	SYS_TIMER = 0;
	//uint32_t delta;
	


	
	frame_buf.buf_idx = 0;
	clearBuffer(&frame_buf);	


	//Enable CTC mode with div by 8
	TCCR0A |= (1 << WGM01); //CTC
	//TCCR0B |= (1 << CS01); //Div by 8
	TCCR0B |= (1 << CS01) | (1 << CS00); //Div by 64

	OCR0A = 68;//1800Hz @ Fclk = 1Mhz/8

	//Interrupt @556us
	TIMSK0 |= (1 << OCIE0A);



	//Set up SYS_CLK timer
	TCCR2A |= (1 << WGM21); 
	//TCCR2B |= (1 << CS20); //Divide by 1 @ 1MHz
	TCCR2B |= (1 << CS21); //Divide by 8 @ 1MHz
	OCR2A = TIMER_MAX;
	TIMSK2 |= (1 << OCIE2A);


	//Setup falling edge interrupt for hall effect sensor
	EICRA |= (1 << ISC01);
	//EIMSK |= (1 << INT0);


	//Setup SPI
	SPCR |= (1 << SPE) | (1 << MSTR);
	SPSR |= (1 << SPI2X);
	PORTB &= ~(CLK_PIN | DATA_PIN | LAT_PIN);


	//Init serial
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0)|(1<<RXCIE0)|(0<<UDRIE0);
    /* Set frame format: 8data, 1stop bit no parity */
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);


	sei();
	_delay_ms(1000);

    //-------EVENT LOOP-----------//
	while (0)
	{
		//clearBuffer(&frame_buf);
		for (int k=0; k<3; k++)
		{
		clearBuffer(&frame_buf);
		for (int i=0; i<FBUF_SZ; i++)
		{
			for (int j=0; j<8; j++)
			{
				frame_buf.buffer[i][j][k] = 0x0F;
			}
		}
			_delay_ms(10);
		}
		//_delay_ms(1000);
	}


	while (0)
	{
		clearBuffer(&frame_buf);
		for (int i=0; i<FBUF_SZ; i++)
		{	
			for (int j=0; j<8; j++)
			{
				if (i < FBUF_SZ/4)
				{
					//Yellow
					setPixel(i, j, 255, 255, 255, &frame_buf);			
				}
				else if (i <FBUF_SZ/2)
				{
					//Magenta
					setPixel(i, j, 255, 255, 255, &frame_buf);
				}
				else if (i < 3*FBUF_SZ/4)
				{
					//CYAN
					setPixel(i, j, 255, 255, 255, &frame_buf);
				}
				else
				{
					//WHITE
					setPixel(i, j, 255, 255, 255, &frame_buf);
				}
			}
		}
		_delay_ms(10000);
	}


    while(1) 
    {
		clearBuffer(&frame_buf);
		for (int i=0; i<FBUF_SZ; i++)
		{
			for (int j=0; j<8; j++)
			{
				switch (i%8)
				{
					case 1:
						frame_buf.buffer[i][j][RED] = 0x40;
						break;
					case 2:
                        frame_buf.buffer[i][j][GREEN] = 0x40;
                        break;
					case 3:
                        frame_buf.buffer[i][j][BLUE] = 0x40;
                        break;
					case 4:
                        frame_buf.buffer[i][j][RED] = 0x40;
						frame_buf.buffer[i][j][GREEN] = 0x40;
                        break;
					case 5:
                        frame_buf.buffer[i][j][RED] = 0x40;
                        frame_buf.buffer[i][j][BLUE] = 0x40;
                        break;
					case 6:
                        frame_buf.buffer[i][j][BLUE] = 0x40;
                        frame_buf.buffer[i][j][GREEN] = 0x40;
                        break;
					case 7:
                        frame_buf.buffer[i][j][RED] = 0x40;
                        frame_buf.buffer[i][j][GREEN] = 0x40;
						frame_buf.buffer[i][j][BLUE] = 0x40;
                        break;
				}
			}
		}
		_delay_ms(5000);
    }
    return(0);

}


void writeDigit(uint8_t digit, int index, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf)
{
    if (digit >= 10)
        return;
    for (int i=0; i<8; i++)
    {
        //int write_idx = index + i;
		int write_idx = index - i;

        if (write_idx >= FBUF_SZ)
            write_idx -= FBUF_SZ;
		if (write_idx < 0)
			write_idx += FBUF_SZ;

      //buf[write_idx] |= NUM_BUF[digit][i];
		//buf[write_idx] |= NUM_BUF[digit][7-i];

//       buf->buffer[z][write_idx] |= pgm_read_byte(&(NUM_BUF[digit][7-i]));

		uint8_t num_data = pgm_read_byte(&(NUM_BUF[digit][7-i]));
		for (int j=0; j<8; j++)
		{
			if ((1 << j) & num_data)
				setPixel(write_idx, j, r, g, b, buf);	
		}
    }
}
void writeNumber(long number, int idx, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf)
{
    if (idx >= FBUF_SZ)
        idx -= FBUF_SZ;
	if (idx < 0)
        idx += FBUF_SZ;
    while(1)
    {
        writeDigit(number%10, idx, r, g, b, buf);
        number /= 10;
        if (number == 0)
            break;
        //idx += 8;
		idx -= 8;
        if (idx >= FBUF_SZ)
            idx -= FBUF_SZ;
		if (idx < 0)
			idx += FBUF_SZ;
    }
}
uint8_t calcIntDelay(uint16_t period)//Period in increments of 250us
{
    //period in uS
    //float out_float = (period/(8*FBUF_SZ)) - 1;
    float out_float = ((period*25)/(48)) - 1; //Simplified for easier math
    int out = (int)(out_float + 0.5);

    if (out > 255)
        return 255;
    if (out <= 5)
        return 5;
    return out;
}
void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf)
{
    if (x < 0 || x >= FBUF_SZ || y < 0 || y >= 8)
        return;

    buf->buffer[x][y][RED] = r;
	buf->buffer[x][y][GREEN] = g;
	buf->buffer[x][y][BLUE] = b;
}
void clearPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, FrameBuffer *buf)
{
    if (x < 0 || x >= FBUF_SZ || y < 0 || y >= 8)
        return;
    buf->buffer[x][y][RED] = 0;
	buf->buffer[x][y][GREEN] = 0;
    buf->buffer[x][y][BLUE] = 0;
}
void clearBuffer(FrameBuffer *buf)
{
	for (int k=0; k<FBUF_SZ; k++)
	{
    	for (int i=0; i<8; i++)
		{
        	buf->buffer[k][i][RED] = 0;
			buf->buffer[k][i][GREEN] = 0;
			buf->buffer[k][i][BLUE] = 0;
		}
	}
}
void setBuffer(FrameBuffer *buf)
{
	for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<8; i++)
        {
            buf->buffer[k][i][RED] = 0xFF;
            buf->buffer[k][i][GREEN] = 0xFF;
            buf->buffer[k][i][BLUE] = 0xFF;
        }
    }
}


