#include <avr/io.h>
#include <util/delay.h>




int main(void) 
{
    //-----------INITS------------//

    DDRC = DDRC | (1 << PC1);
    PORTC = (1 << PC1);


    DDRB |= (1 << PB0) | (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
    DDRC |= (1 << PC0);

    PORTB &= ~((1 << PB0) | (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7));
    PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
    PORTC &= ~(1 << PC0);


    



    //-------EVENT LOOP-----------//
    while(1) 
    {
        PORTC ^=  (1 << PC1);
        _delay_ms(500);        //Turn on LED in PORTB

    }
    return(0);

}
