#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "delay.h"
#include "RingBuffer.h"


volatile RingBuffer ringbuffer;


uint8_t serialDataAvailable()
{
    return (UCSR0A & (1<<RXC0));
}

ISR(USART_RX_vect)
{
//    uint8_t input = receiveByte();

//    PORTB = 0x01;
//    delay_ms(input*10);
//    PORTB = 0;

    while (serialDataAvailable())
    {
        pushRingBuf(&ringbuffer, receiveByte());
        _delay_us(100);
    }
}


int main(void) 
{
    //-----------INITS------------//

    DDRB |= 0x03;
    PORTB = 0x01;
    _delay_ms(1000);
    PORTB = 0x03;
    _delay_ms(1000);
    PORTB = 0x00;    

    initRingBuf(&ringbuffer);

    initUSART();
    UCSR0B |= (1 << RXCIE0);

    while (serialDataAvailable())
        receiveByte();

    sei();

    //-------EVENT LOOP-----------//
    while(1) 
    {
        if (!ringbuffer.isEmpty)
        {
            uint8_t val = popRingBuf(&ringbuffer);
            PORTB = 0x01;
            delay_ms(val*10);
            PORTB = 0x00;
        }
        _delay_ms(250);
        PORTB = 0x02;   
        _delay_ms(250);
        PORTB = 0x00;
    }
    return(0);

}
