#include <avr/io.h>
#include <util/delay.h>

int main(void) 
{
    //-----------INITS------------//

    DDRB |= 0x03;
    PORTB = 0b00000011;


    //-------EVENT LOOP-----------//
    while(1) 
    {
        //PORTB = (PORTB << 1)|(PORTB >> 7);
        //PORTB ^= 0b11111111;
        PORTB ^= 0x03;
        _delay_ms(50);        //Turn on LED in PORTB

//        PORTB = 0b00000000;     //Turn off LED in PORTB
//        _delay_ms(100);
    }
    return(0);

}
