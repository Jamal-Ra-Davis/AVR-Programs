#include <avr/io.h>
#include <util/delay.h>

int main(void) 
{
    //-----------INITS------------//

    DDRB |= 0x01;
    PORTB = 0b00000001;


    //-------EVENT LOOP-----------//
    while(1) 
    {
        //PORTB = (PORTB << 1)|(PORTB >> 7);
        //PORTB ^= 0b11111111;
        PORTB ^= 0x01;
        _delay_ms(500);        //Turn on LED in PORTB

//        PORTB = 0b00000000;     //Turn off LED in PORTB
//        _delay_ms(100);
    }
    return(0);

}
