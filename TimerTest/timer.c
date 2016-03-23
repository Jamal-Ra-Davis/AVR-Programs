#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void initTimer()
{
    //init timer 1
    cli();              //disable global interrupts
    TCCR0A = 0;         //Clear TTR1 registers
    TCCR0B = 0;

    //Enable timer1 overflow interrupt
    TIMSK0 = (1 << TOIE0);
    //TIMSK0 = (1 << OCIE0A);

    //Set CS10 bit so timer runs at 1/64th clock speed
    //TCCR1B |= ((1 << CS11) | (1 << CS10));// 1/64 clock speed
    //TCCR0B |= (1 << CS00); //No Scaling 1 MHz
    //TCCR1B |= (1 << CS11); // 1/8 clock speed
    TCCR0B |= ((1 << CS01)|(0 << CS00));

    //uint16_t val = 60;
    //OCR1A = val;
    //OCR1A = 150*8*4;
    sei();
}
/*
ISR(TIMER1_COMPA_vect)
{
    PORTB ^= 0x01;  
}
*/

ISR(TIMER0_OVF_vect) {
    PORTB ^= 0x01;
}

int main(void) 
{
    //-----------INITS------------//
    DDRB |= 0x01;
    PORTB = 0x00;
    initTimer();

    //-------EVENT LOOP-----------//
    while(1) 
    {
    }
    return(0);

}
