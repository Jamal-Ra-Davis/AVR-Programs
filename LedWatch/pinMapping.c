#include "pinMapping.h"


void setPinmode(uint8_t pin, uint8_t direction)
{
	switch(pin)
	{
		case 1:
		{
			if (direction)
				DDRC |= (1 << PC6);
			else
				DDRC &= ~(1 << PC6);
			break;
		}
		case 2:
		{
                        if (direction)
                                DDRD |= (1 << PD0);
                        else
                                DDRD &= ~(1 << PD0);
                        break;
                }
		case 3:
		{
                        if (direction)
                                DDRD |= (1 << PD1);
                        else
                                DDRD &= ~(1 << PD1);
                        break;
                }
		case 4:
		{
                        if (direction)
                                DDRD |= (1 << PD2);
                        else
                                DDRD &= ~(1 << PD2);
                        break;
                }
		case 5:
		{
                        if (direction)
                                DDRD |= (1 << PD3);
                        else
                                DDRD &= ~(1 << PD3);
                        break;
                }
		case 6:
		{
                        if (direction)
                                DDRD |= (1 << PD4);
                        else
                                DDRD &= ~(1 << PD4);
                        break;
                }
		case 9:
		{
			if (direction)
                                DDRB |= (1 << PB6);
                        else
                                DDRB &= ~(1 << PB6);
                        break;
		}
		case 10:
		{
			if (direction)
                                DDRB |= (1 << PB7);
                        else
                                DDRB &= ~(1 << PB7);
                        break;
		}
		case 11:
                {
                        if (direction)
                                DDRD |= (1 << PD5);
                        else
                                DDRD &= ~(1 << PD5);
                        break;
                }
		case 12:
                {
                        if (direction)
                                DDRD |= (1 << PD6);
                        else
                                DDRD &= ~(1 << PD6);
                        break;
                }
		case 13:
                {
                        if (direction)
                                DDRD |= (1 << PD7);
                        else
                                DDRD &= ~(1 << PD7);
                        break;
                }
		case 14:
                {
                        if (direction)
                                DDRB |= (1 << PB0);
                        else
                                DDRB &= ~(1 << PB0);
                        break;
                }
		case 15:
                {
                        if (direction)
                                DDRB |= (1 << PB1);
                        else
                                DDRB &= ~(1 << PB1);
                        break;
                }
		case 16:
                {
                        if (direction)
                                DDRB |= (1 << PB2);
                        else
                                DDRB &= ~(1 << PB2);
                        break;
                }
		case 17:
                {
                        if (direction)
                                DDRB |= (1 << PB3);
                        else
                                DDRB &= ~(1 << PB3);
                        break;
                }
		case 18:
                {
                        if (direction)
                                DDRB |= (1 << PB4);
                        else
                                DDRB &= ~(1 << PB4);
                        break;
                }
		case 19:
                {
                        if (direction)
                                DDRB |= (1 << PB5);
                        else
                                DDRB &= ~(1 << PB5);
                        break;
                }
		case 23:
		{
			if (direction)
                                DDRC |= (1 << PC0);
                        else
                                DDRC &= ~(1 << PC0);
                        break;
		}
		case 24:
                {
                        if (direction)
                                DDRC |= (1 << PC1);
                        else
                                DDRC &= ~(1 << PC1);
                        break;
                }
		case 25:
                {
                        if (direction)
                                DDRC |= (1 << PC2);
                        else
                                DDRC &= ~(1 << PC2);
                        break;
                }
		case 26:
                {
                        if (direction)
                                DDRC |= (1 << PC3);
                        else
                                DDRC &= ~(1 << PC3);
                        break;
                }
		case 27:
                {
                        if (direction)
                                DDRC |= (1 << PC4);
                        else
                                DDRC &= ~(1 << PC4);
                        break;
                }
		case 28:
                {
                        if (direction)
                                DDRC |= (1 << PC5);
                        else
                                DDRC &= ~(1 << PC5);
                        break;
                }
		
	}
}
void digitalWrite(uint8_t pin, uint8_t val)
{
	switch(pin)
	{
		case 1:
		{
			if (val)
				PORTC |= (1 << PC6);
			else
				PORTC &= ~(1 << PC6);
			break;
		}
		case 2:
		{
                        if (val)
                                PORTD |= (1 << PD0);
                        else
                                PORTD &= ~(1 << PD0);
                        break;
                }
		case 3:
		{
                        if (val)
                                PORTD |= (1 << PD1);
                        else
                                PORTD &= ~(1 << PD1);
                        break;
                }
		case 4:
		{
                        if (val)
                                PORTD |= (1 << PD2);
                        else
                                PORTD &= ~(1 << PD2);
                        break;
                }
		case 5:
		{
                        if (val)
                                PORTD |= (1 << PD3);
                        else
                                PORTD &= ~(1 << PD3);
                        break;
                }
		case 6:
		{
                        if (val)
                                PORTD |= (1 << PD4);
                        else
                                PORTD &= ~(1 << PD4);
                        break;
                }
		case 9:
		{
			if (val)
                                PORTB |= (1 << PB6);
                        else
                                PORTB &= ~(1 << PB6);
                        break;
		}
		case 10:
		{
			if (val)
                                PORTB |= (1 << PB7);
                        else
                                PORTB &= ~(1 << PB7);
                        break;
		}
		case 11:
                {
                        if (val)
                                PORTD |= (1 << PD5);
                        else
                                PORTD &= ~(1 << PD5);
                        break;
                }
		case 12:
                {
                        if (val)
                                PORTD |= (1 << PD6);
                        else
                                PORTD &= ~(1 << PD6);
                        break;
                }
		case 13:
                {
                        if (val)
                                PORTD |= (1 << PD7);
                        else
                                PORTD &= ~(1 << PD7);
                        break;
                }
		case 14:
                {
                        if (val)
                                PORTB |= (1 << PB0);
                        else
                                PORTB &= ~(1 << PB0);
                        break;
                }
		case 15:
                {
                        if (val)
                                PORTB |= (1 << PB1);
                        else
                                PORTB &= ~(1 << PB1);
                        break;
                }
		case 16:
                {
                        if (val)
                                PORTB |= (1 << PB2);
                        else
                                PORTB &= ~(1 << PB2);
                        break;
                }
		case 17:
                {
                        if (val)
                                PORTB |= (1 << PB3);
                        else
                                PORTB &= ~(1 << PB3);
                        break;
                }
		case 18:
                {
                        if (val)
                                PORTB |= (1 << PB4);
                        else
                                PORTB &= ~(1 << PB4);
                        break;
                }
		case 19:
                {
                        if (val)
                                PORTB |= (1 << PB5);
                        else
                                PORTB &= ~(1 << PB5);
                        break;
                }
		case 23:
		{
			if (val)
                                PORTC |= (1 << PC0);
                        else
                                PORTC &= ~(1 << PC0);
                        break;
		}
		case 24:
                {
                        if (val)
                                PORTC |= (1 << PC1);
                        else
                                PORTC &= ~(1 << PC1);
                        break;
                }
		case 25:
                {
                        if (val)
                                PORTC |= (1 << PC2);
                        else
                                PORTC &= ~(1 << PC2);
                        break;
                }
		case 26:
                {
                        if (val)
                                PORTC |= (1 << PC3);
                        else
                                PORTC &= ~(1 << PC3);
                        break;
                }
		case 27:
                {
                        if (val)
                                PORTC |= (1 << PC4);
                        else
                                PORTC &= ~(1 << PC4);
                        break;
                }
		case 28:
                {
                        if (val)
                                PORTC |= (1 << PC5);
                        else
                                PORTC &= ~(1 << PC5);
                        break;
                }
		
	}
}

