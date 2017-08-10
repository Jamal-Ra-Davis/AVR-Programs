#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FBUF_SZ 60
#define LED_PORT PORTB
#define LED_DDR	DDRB
#define TIMER_MAX 249

#define DEFAULT_INT_DELAY 68

int buf_idx;
uint8_t fbuf[FBUF_SZ];

uint64_t SYS_TIMER;
uint64_t time_delta;


uint8_t NUM_BUF[10][8] = {
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
void writeDigit(uint8_t digit, int index, uint8_t *buf)
{
	if (digit >= 10)
		return;
	for (int i=0; i<8; i++)
	{
		int write_idx = index + i;
		if (write_idx >= FBUF_SZ)
			write_idx -= FBUF_SZ;

		buf[write_idx] |= NAME_BUF[digit][i];
	}
}
uint8_t calcIntDelay(int period)//Period in increments of 250us
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

ISR(INT0_vect)
{
	time_delta = SYS_TIMER - time_delta;
	if (time_delta >= 150)
		OCR0A = DEFAULT_INT_DELAY;
	else
		OCR0A = calcIntDelay(time_delta);
	TCNT0 = 0;
	//LED_PORT ^= 0xFF;
	buf_idx = 0;
	LED_PORT = fbuf[buf_idx];
}
ISR(TIMER0_COMPA_vect)
{
	LED_PORT = fbuf[buf_idx];
	buf_idx++;
	if (buf_idx >= FBUF_SZ)
		buf_idx = 0;	
}
ISR(TIMER2_COMPA_vect)
{
	SYS_TIMER++;
}
void setPixel(int x, int y, uint8_t *buf)
{
	if (x < 0 || x >= FBUF_SZ || y < 0 || y >= 8)
		return;
	buf[x] |= (1 << y);
}
void clearPixel(int x, int y, uint8_t *buf)
{
    if (x < 0 || x >= FBUF_SZ || y < 0 || y >= 8)
        return;
    buf[x] &= ~(1 << y);
}
void clearBuffer(uint8_t *buf)
{
	for (int i=0; i<FBUF_SZ; i++)
		buf[i] = 0x00;
}
void setBuffer(uint8_t *buf)
{
    for (int i=0; i<FBUF_SZ; i++)
        buf[i] = 0xFF;
}
int main(void) 
{
    //-----------INITS------------//
	LED_DDR = 0xFF;
	LED_PORT = 0x00;
	int i=0;
	uint8_t toggle = 0;
	SYS_TIMER = 0;


	buf_idx = 0;
	for (int j=0; j<FBUF_SZ; j++)
		fbuf[j] = 0;

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

	sei();
	time_delta = SYS_TIMER - 0;


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
