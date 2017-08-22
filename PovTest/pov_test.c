#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 1000000UL

#define FBUF_SZ 60
#define FBUF_HEIGHT 3
#define LED_PORT PORTB
#define LED_DDR	DDRB
#define TIMER_MAX 249

#define CLK_PIN (1 << PB5)
#define LAT_PIN (1 << PB2)
#define DATA_PIN (1 << PB3)

#define DEFAULT_INT_DELAY 68

typedef struct
{
    uint8_t buffer[FBUF_HEIGHT][FBUF_SZ];
	int buf_idx;
} FrameBuffer;


int buf_idx;
uint8_t fbuf[FBUF_SZ];

uint32_t SYS_TIMER;
uint32_t time_delta;


FrameBuffer frame_buf;


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
						/* 
						{
							{0x00, 0x18, 0x24, 0x24, 0x24, 0x24, 0x18, 0x00},//0
							{0x00, 0x1C, 0x08, 0x08, 0x08, 0x18, 0x08, 0x00},//1
							{0x00, 0x3C, 0x20, 0x10, 0x08, 0x24, 0x18, 0x00},//2
							{0x00, 0x38, 0x04, 0x04, 0x18, 0x04, 0x38, 0x00},//3
							{0x00, 0x08, 0x08, 0x3C, 0x28, 0x28, 0x28, 0x00},//4
							{0x00, 0x38, 0x04, 0x04, 0x38, 0x20, 0x3C, 0x00},//5
							{0x00, 0x18, 0x24, 0x24, 0x38, 0x20, 0x1C, 0x00},//6
							{0x00, 0x10, 0x10, 0x10, 0x08, 0x04, 0x3C, 0x00},//7
							{0x00, 0x18, 0x24, 0x24, 0x18, 0x24, 0x18, 0x00},//8
							{0x00, 0x04, 0x04, 0x1C, 0x24, 0x24, 0x18, 0x00} //9
						};
						*/
void writeDigit(uint8_t digit, int index, int z, FrameBuffer *buf);
void writeNumber(long number, int idx, int z, FrameBuffer *buf);
uint8_t calcIntDelay(uint16_t period);
void setPixel(int x, int y, int z, FrameBuffer *buf);
void clearPixel(int x, int y, int z, FrameBuffer *buf);
void clearBuffer(FrameBuffer *buf);
void setBuffer(FrameBuffer *buf);

uint8_t spi_tranceiver(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}


ISR(INT0_vect)//Hall-effect trigger
{
	uint32_t delta = SYS_TIMER - time_delta;
	time_delta = SYS_TIMER;
	if (delta >= 1000)
		OCR0A = DEFAULT_INT_DELAY;
	else if (delta >= 491)
		OCR0A = 255;
	else if (delta <= 32)
		OCR0A = 16;
	else
	{
		//OCR0A = calcIntDelay(delta);

		uint32_t val = 533*delta - 512;//== 533*delta + 512 - 1024
		/*
		if (val % 1024U < 512)
		{
			val = val >> 10;
			val--;
		}
		else
		{
			val = val >> 10;
		}
		*/
		OCR0A = (uint8_t)(val >> 10);
	}

	TCNT0 = 0;
	buf_idx = 0;
	//LED_PORT = fbuf[buf_idx];
}
int w_idx, b_idx;
ISR(TIMER0_COMPA_vect)
{
	//LED_PORT = fbuf[buf_idx];

	w_idx = buf_idx + 20;
	if (w_idx >= FBUF_SZ)
		w_idx -= FBUF_SZ;

	b_idx = buf_idx + 40;
	if (b_idx >= FBUF_SZ)
		b_idx -= FBUF_SZ;


	LED_PORT &= ~LAT_PIN;
	//spi_tranceiver(fbuf[buf_idx]);
	spi_tranceiver(frame_buf.buffer[2][b_idx]);
	spi_tranceiver(frame_buf.buffer[1][w_idx]);
	spi_tranceiver(frame_buf.buffer[0][buf_idx]);
	LED_PORT |= LAT_PIN;

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
	LED_DDR = 0xFF;
	LED_PORT = 0x00;
	int i=0;
	uint8_t toggle = 0;
	SYS_TIMER = 0;
	uint32_t delta;
	

	buf_idx = 0;
	for (int j=0; j<FBUF_SZ; j++)
		fbuf[j] = 0;

	
	frame_buf.buf_idx = 0;	
	for (int j=0; j<FBUF_SZ; j++)
	{
		frame_buf.buffer[0][j] = 0;
		frame_buf.buffer[1][j] = 0;
		frame_buf.buffer[2][j] = 0;
	}
	for (int j=0; j<FBUF_SZ; j++)
	{
		if (j < 25)
		{
			frame_buf.buffer[0][j] = 0x05;
			frame_buf.buffer[1][j] = 0x0A;
			frame_buf.buffer[2][j] = 0xF0;

			if (j != 0 && j%4 == 0)
			{
				uint8_t mask = 0xA0;
				frame_buf.buffer[1][j] |= mask;
				frame_buf.buffer[2][j] &= ~mask;
			}
		}
		else
		{
			frame_buf.buffer[0][j] = 0x55;
            frame_buf.buffer[1][j] = 0xAA;
		}
	} 
	for (int j=50; j<FBUF_SZ; j++)
	{
		frame_buf.buffer[0][j] = 0;
        frame_buf.buffer[1][j] = 0;
        frame_buf.buffer[2][j] = 0;	
	}	


	//Enable CTC mode with div by 8
	TCCR0A |= (1 << WGM01); //CTC
	TCCR0B |= (1 << CS01); //Div by 8

	OCR0A = 68;//1800Hz @ Fclk = 1Mhz/8

	//Interrupt @556us
	TIMSK0 |= (1 << OCIE0A);


	//Set up SYS_CLK timer
	TCCR2A |= (1 << WGM21); 
	TCCR2B |= (1 << CS20); //Divide by 1 @ 1MHz
	OCR2A = TIMER_MAX;
	TIMSK2 |= (1 << OCIE2A);


	//Setup falling edge interrupt for hall effect sensor
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);


	SPCR |= (1 << SPE) | (1 << MSTR);
	PORTB &= ~(CLK_PIN | DATA_PIN | LAT_PIN);
	sei();
	
//	while(1)//Freedom loop
//	{
		_delay_ms(1000);
//	}

	
	//Shift register test
/*
	uint8_t sr_val[3] = {0x81, 0xF0, 0xAA};
	

	while(1)
	{
		PORTB &= ~LAT_PIN;
		for (int i=0; i<3; i++)
		{
			spi_tranceiver(sr_val[2-i]);
		}	
        PORTB |= LAT_PIN;
        _delay_ms(500);

		PORTB &= ~LAT_PIN;
		spi_tranceiver(0);
		spi_tranceiver(0);
		spi_tranceiver(0);
        PORTB |= LAT_PIN;
        _delay_ms(500);
	}

	while (1)
	{
		PORTB &= ~LAT_PIN;
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<8; j++)
			{
				PORTB &= ~CLK_PIN;
				if (sr_val[0] & (1 << (7-j)))
					PORTB |= DATA_PIN;
				else
					PORTB &= ~DATA_PIN;
				PORTB |= CLK_PIN;
			}
			//sr_val[3-i] = ~sr_val[3-i];
			sr_val[0] = ~sr_val[0];
		}	
		PORTB |= LAT_PIN;
		_delay_ms(500);
	}
*/


	//Write time delta after "1000ms" delay
	delta = SYS_TIMER - 0;
	clearBuffer(&frame_buf);
	for (int i=0; i<FBUF_SZ; i++)
		frame_buf.buffer[0][i] |= 0x01;	
	writeNumber(delta, -5, 0, &frame_buf);
    writeNumber(delta, -5, 1, &frame_buf);
	writeNumber(delta, -5, 2, &frame_buf);
    _delay_ms(5000);
	i=0;
	delta = SYS_TIMER;
    //-------EVENT LOOP-----------//
    while(1) 
    {
		/*
		if ((PIND & (1 << PD2)) == 0)
		{
			//for (int i=0; i<FBUF_SZ; i++)
			//	fbuf[i] = 0x00;
			//_delay_ms(100);
			//continue;

			LED_PORT = 0xFF;
		}
		else
		{
			LED_PORT = 0x00;
		}
		//_delay_ms(100);
		continue;
		*/
/*
		//Test that numerical display works
		for (int i=0; i<256; i++)
		{
			clearBuffer(&frame_buf);
			writeNumber(i, 15, 2, &frame_buf);
			_delay_ms(250);
		}
		//writeNumber(735, 15, fbuf);
		//_delay_ms(1000);
		continue;
		//write out time delta from interrupt
		//time_delta = SYS_TIMER - time_delta;
*/		

		delta = SYS_TIMER-delta;
		clearBuffer(&frame_buf);
		writeNumber(delta, 30, 2, &frame_buf);
		writeNumber(i, 15, 1, &frame_buf);
		i++;
		delta = SYS_TIMER;
		while((SYS_TIMER-delta) < 2000)
		{
			_delay_us(50);
		}
		//time_delta = SYS_TIMER;
		//_delay_ms(500);
		continue;

		//Do snazzy rotating animation with 0 position marker
		for (int j=0; j<8; j++)
		{
			for (i=0; i<FBUF_SZ; i++)
        	{
				fbuf[i] = (1 << (i+j)%8);
        	}
			fbuf[FBUF_SZ - 3] = 0x03;
			fbuf[FBUF_SZ - 2] = 0x0F;
			fbuf[FBUF_SZ - 1] = 0x3F;
			fbuf[0] = 0xFF;	
			fbuf[1] = 0x3F;
			fbuf[2] = 0x0F;
			fbuf[3] = 0x03;
			_delay_ms(500);
		}
		continue;

		//Draw triangle wave basically
		for (i=0; i<8; i++)
		{
			LED_PORT = (1 << i);
			_delay_us(500);
		}
		for (i=6; i>=1; i--)
		{
			LED_PORT = (1 << i);
            _delay_us(500);
		}
		continue;

		LED_PORT = (1 << i);
		if (toggle)
		{
			i++;
			if (i == 7)
				toggle ^= 0x01;
		}
		else
		{
			i--;
			if (i == 0)
				toggle ^= 0x01;
		}
		_delay_ms(250);	
    }
    return(0);

}



void writeDigit(uint8_t digit, int index, int z, FrameBuffer *buf)
{
	if (z >= FBUF_HEIGHT || z < 0)
		return;
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
        buf->buffer[z][write_idx] |= pgm_read_byte(&(NUM_BUF[digit][7-i]));

    }
}
void writeNumber(long number, int idx, int z, FrameBuffer *buf)
{
	if (z >= FBUF_HEIGHT || z < 0)
        return;
    if (idx >= FBUF_SZ)
        idx -= FBUF_SZ;
	if (idx < 0)
        idx += FBUF_SZ;
    while(1)
    {
        writeDigit(number%10, idx, z, buf);
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
void setPixel(int x, int y, int z, FrameBuffer *buf)
{
    if (x < 0 || x >= FBUF_SZ || y < 0 || y >= 8 || z >= FBUF_HEIGHT || z < 0)
        return;
    buf->buffer[z][x] |= (1 << y);
}
void clearPixel(int x, int y, int z, FrameBuffer *buf)
{
    if (x < 0 || x >= FBUF_SZ || y < 0 || y >= 8 || z >= FBUF_HEIGHT || z < 0)
        return;
    buf->buffer[z][x] &= ~(1 << y);
}
void clearBuffer(FrameBuffer *buf)
{
	for (int k=0; k<FBUF_HEIGHT; k++)
	{
    	for (int i=0; i<FBUF_SZ; i++)
        	buf->buffer[k][i] = 0x00;
	}
}
void setBuffer(FrameBuffer *buf)
{
	for (int k=0; k<FBUF_HEIGHT; k++)
	{
    	for (int i=0; i<FBUF_SZ; i++)
        	buf->buffer[k][i] = 0xFF;
	}
}


