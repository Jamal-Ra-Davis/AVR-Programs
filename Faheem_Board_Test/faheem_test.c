#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "simple_json.h"
#include "delay.h"
#include "USART.h"



void USARTInit()
{
    /*Set baud rate */
    UBRR0H = (BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    /*Enable receiver and transmitter and Receive interupt and Data Register Empty*/
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<UDRIE0);

    /* Set frame format: 8data, 1stop bit no parity */
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}
void setSerialRecieveInterrupt(uint8_t setting)
{
    if (setting)
        UCSR0B |= (1 << RXCIE0);
    else
        UCSR0B &= ~(1 << RXCIE0);
}
uint8_t serialDataAvailable()
{
    return (UCSR0A & (1<<RXC0));
}
ISR(USART_RX_vect)
{
    while (serialDataAvailable())
    {
        //pushRingBuf(&ringbuffer, receiveByte());
        _delay_us(800);
    }
}



int main(void) 
{
    //-----------INITS------------//
	DDRB = 0x03;
	PORTB = 0x00;

	initUSART();

	delay_ms(1000);

	const char* json = "{\"Blink Time\" : 1000}";

	JSON_Data packet;

	int d_time = 10000;

	int flag = simpleJsonParse(json, strlen(json), &packet);	
	if (flag)
	{
		PORTB = 0x02;
	}	
	else
	{
		PORTB = 0x01;
		sscanf(packet.value, "%d", &d_time);
	}

	_delay_ms(1000);
	PORTB = 0x00;
    //-------EVENT LOOP-----------//
    while(1) 
    {
		PORTB ^= 0x02;
		delay_ms(d_time);
    }
    return(0);

}
