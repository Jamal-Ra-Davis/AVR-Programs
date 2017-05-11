#include <util/delay.h>
#include <avr/io.h>
#include "delay.h"


void delay_ms(uint16_t ms)
{
    while(ms--)
    {
        _delay_ms(1);
    }
}
void timer_delay_ms(uint16_t ms)
{
    uint8_t cmp = 126*ms;
    TCNT1 = 0;
    while(TCNT1 < cmp);
}
void delay_us(uint16_t us)
{
    while(us--)
    {
        _delay_us(1);
    }
}
