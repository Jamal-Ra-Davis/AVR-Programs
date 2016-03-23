#include <avr/io.h>
#include <util/delay.h>


void POVDisplay(uint8_t oneByte)
{
    PORTB = oneByte;
    _delay_ms(2);
}

int main(void) 
{
    //-----------INITS------------//

    DDRB |= 0xFF;//Sets PORTB as output
    DDRD |= 0x00;//Set PORTB pins all as input
    PORTD |= (1 << PD2);//Inits pullup resistor on pin2 of portB

    PORTB = 0x00;//Turns leds off initially

    //-------EVENT LOOP-----------//
    while(1) 
    {
        if (PIND & (1 << PD2))
            PORTB = 0x00;
        else
            PORTB = 0xFF;
    }
    return(0);

}
