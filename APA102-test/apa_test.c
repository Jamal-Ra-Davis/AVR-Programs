#include <avr/io.h>
#include <util/delay.h>

#define RED 0
#define GREEN 1
#define BLUE 2

uint8_t leds[8][3];

uint8_t spi_tranceiver(uint8_t byte)
{
    SPDR = byte;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void updateLeds()
{
	//Start frame
	spi_tranceiver(0x00);
	spi_tranceiver(0x00);
	spi_tranceiver(0x00);
	spi_tranceiver(0x00);


	//led frames
	for (int i=0; i<8; i++)
	{
		spi_tranceiver(0xFF);
		spi_tranceiver(leds[i][BLUE]);
		spi_tranceiver(leds[i][GREEN]);
		spi_tranceiver(leds[i][RED]);
	}

	//end frame
	for (int i=0; i<8; i += 16)
	{
		spi_tranceiver(0xFF);
	}
}

void clearLeds()
{
	for (int i=0; i<8; i++)
    {
        leds[i][RED] = 0;
        leds[i][GREEN] = 0;
        leds[i][BLUE] = 0;
    }
}

int main(void) 
{
    //-----------INITS------------//
	DDRB = (1 << PB2) | (1 << PB5) | (1 << PB3);

	//Setup SPI
    SPCR |= (1 << SPE) | (1 << MSTR);
    SPSR |= (1 << SPI2X);

	PORTB = 0x00;

	clearLeds();



    //-------EVENT LOOP-----------//
    while(1) 
    {
		for (int i=0; i<3; i++)
		{
			clearLeds();
			for (int j=0; j<8; j++)
			{
				leds[j][i] = 0x0F;
			} 
			updateLeds();
			if (i == 0)
				_delay_ms(2000);
			else 
				_delay_ms(1000);
		}
	
		for (int i=0; i<8; i++)
		{
			clearLeds();
			leds[i][RED] = 0x0F;
			leds[i][GREEN] = 0x0F;
			leds[i][BLUE] = 0x0F;
			updateLeds();
			_delay_ms(500);
		}


    }
    return(0);

}
