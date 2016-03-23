#include <avr/io.h>
#include <util/delay.h>
#include "USART.h"

void POVDisplay(uint8_t oneByte)
{
    PORTB = oneByte;
    _delay_ms(2);
}

int main(void) 
{
    //-----------INITS------------//

    DDRB |= 0x01;
    PORTB = 0x00;
    initUSART();

    PORTB = 0x01;
    _delay_ms(100);
    PORTB = 0x00;
    _delay_ms(100);
    PORTB = 0x01;
    _delay_ms(1000);
    PORTB = 0x00;

    //-------EVENT LOOP-----------//
    while(1) 
    {  
        
        char in = receiveByte();
        /*
        if (in == 'h')
            PORTB = 0x01;
        else if (in == 'l')
            PORTB = 0x00;
        */
        PORTB = 0x01;
        for (int i=0; i<in; i++)
            _delay_ms(1);
        PORTB = 0x00;
    }
    return(0);

}
