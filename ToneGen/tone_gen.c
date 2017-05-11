#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "notes.h"

#define PRESCALE_1 (1 << CS00)
#define PRESCALE_8 (1 << CS01)
#define PRESCALE_64 (1 << CS01) | (1 << CS00)
#define PRESCALE_256 (1 << CS02)
#define PRESCALE_1024 (1 << CS02) | (1 << CS00)

int HZtoN(int hz)
{
	if (hz <= 0)
		return 255;
	uint16_t val = 62500;
	val = val/hz - 1;
	if (val > 255)
		return 255;
	if (val <= 0)
		return 1;

	return val;
	
}

int main(void) 
{
    //-----------INITS------------//
	clock_prescale_set(clock_div_1);

	DDRD |= (1 << PD6) | (1 << PD3);
	DDRB |= (1 << PB1) | (1 << PB3);
	
	

	
	TCNT0 = 0;
	OCR0A = 124;
	
	TCCR0A |= (1 << COM0A0) | (1 << WGM01);
	TCCR0B |= PRESCALE_64;



	TCNT1 = 0;
	OCR1A = 666;

	TCCR1A |= (1 << COM1A0);
	TCCR1B |= (1 << WGM12) | (1 << CS10);


	TCNT2 = 0;
	OCR2A = 50;
	OCR2B = 200;

	TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
	TCCR2B |= (1 << CS20);



	int i=0;
	int top[2] = {124, 147};

	top[0] = HZtoN(500);
	top[1] = HZtoN(420);


	int song[12][2] = {
						{E5, 1},
						{E5, 1},
						{0, 1},
						{E5, 1},
						{0, 1},
						{C4, 1},
						{E5, 1},
						{0, 1},
						{G5, 1},
						{0, 3},
						{G4, 1},
						{0, 3},
					};
			
/*	
	while (1)
	{		
		int loop;
		for (loop=0; loop<12; loop++)
		{
			DDRD &= ~(1 << PD6);
			_delay_ms(32);

			if (song[loop][0])
			{
				OCR0A = HZtoN(song[loop][0]);
				DDRD |= (1 << PD6);
			}
			int delay_;
			for (delay_=0; delay_<song[loop][1]; delay_++)
			{
				_delay_ms(1600);
			}		
		}
	}
*/

    //-------EVENT LOOP-----------//
    while(1) 
    {
		OCR0A = top[i];
		i++;
		if (i>=2)
			i=0;
		_delay_ms(30);	
		//_delay_ms(250);
    }
    return(0);

}
