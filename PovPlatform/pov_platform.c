#include <avr/io.h>
#include <util/delay.h>


void POVDisplay(uint8_t oneByte)
{
    PORTD = oneByte;
    _delay_ms(2);
}

int main(void) 
{
    //-----------INITS------------//

    DDRD |= 0xFF;
    PORTD = 0x00;

    uint8_t i = 0;
    //-------EVENT LOOP-----------//
    while(1) 
    {
    /*
        PORTD = (1 << i);
        i++;
        if (i >= 6)
            i = 0;
        _delay_ms(250);
    */
/*
        POVDisplay(0b00001110);
        POVDisplay(0b00011000);
        POVDisplay(0b10111101);
        POVDisplay(0b01110110);
        POVDisplay(0b00111100);
        POVDisplay(0b00111100);
        POVDisplay(0b00111100);
        POVDisplay(0b01110110);
        POVDisplay(0b10111101);
        POVDisplay(0b00011000);
        POVDisplay(0b00001110);
*/
/*
        POVDisplay(0b00111111);
        POVDisplay(0b00100001);
        POVDisplay(0b00111111);

        POVDisplay(0b00000000);

        POVDisplay(0b00000001);
        POVDisplay(0b00000001);
        POVDisplay(0b00111111);

        POVDisplay(0b00000000);

        POVDisplay(0b00000001);
        POVDisplay(0b00000001);
        POVDisplay(0b00111111);

        POVDisplay(0b00000000);

        POVDisplay(0b00101101);
        POVDisplay(0b00101101);
        POVDisplay(0b00111111);

        POVDisplay(0b00000000);

        POVDisplay(0b00111111);
        POVDisplay(0b00001100);
        POVDisplay(0b00111111);
*/
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000001);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);

        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000001);
        POVDisplay(0b00000011);
        POVDisplay(0b00000001);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);

        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000001);
        POVDisplay(0b00000011);
        POVDisplay(0b00000111);
        POVDisplay(0b00000011);
        POVDisplay(0b00000001);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);

        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000001);
        POVDisplay(0b00000011);
        POVDisplay(0b00000111);
        POVDisplay(0b00001111);
        POVDisplay(0b00000111);
        POVDisplay(0b00000011);
        POVDisplay(0b00000001);
        POVDisplay(0b00000000);
        POVDisplay(0b00000000);

        POVDisplay(0b00000000);
        POVDisplay(0b00000000);
        POVDisplay(0b00000001);
        POVDisplay(0b00000011);
        POVDisplay(0b00000111);
        POVDisplay(0b00001111);
        POVDisplay(0b00011111);
        POVDisplay(0b00001111);
        POVDisplay(0b00000111);
        POVDisplay(0b00000011);
        POVDisplay(0b00000001);
        POVDisplay(0b00000000);

        POVDisplay(0b00000000);
        POVDisplay(0b00000001);
        POVDisplay(0b00000011);
        POVDisplay(0b00000111);
        POVDisplay(0b00001111);
        POVDisplay(0b00011111);
        POVDisplay(0b00111111);
        POVDisplay(0b00011111);
        POVDisplay(0b00001111);
        POVDisplay(0b00000111);
        POVDisplay(0b00000011);
        POVDisplay(0b00000001);

        POVDisplay(0b00000000);
        POVDisplay(0b00000010);
        POVDisplay(0b00000111);
        POVDisplay(0b00001111);
        POVDisplay(0b00011111);
        POVDisplay(0b00111111);
        POVDisplay(0b01111111);
        POVDisplay(0b00111111);
        POVDisplay(0b00011111);
        POVDisplay(0b00001111);
        POVDisplay(0b00000111);
        POVDisplay(0b00000010);
    }
    return(0);

}
