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

    //DDRB |= 0xFF;
    DDRD |= (1 << PD6);
    OCR0A = 255;
    
    uint8_t brightness = 0;

    TCCR0A |= (1 << COM0A1)|(1 << COM0A0);//Non inverting mode

    TCCR0A |= (1 << WGM01) | (1 << WGM00);//Fast pwm

    TCCR0B |= (1 << CS01);//Set prescalar to 8 and starts pwm

    //-------EVENT LOOP-----------//
    while(1) 
    {
        OCR0A = brightness;
        brightness += 15;
        _delay_ms(200);
    }
    return(0);

}
