#include <avr/io.h>
#include <util/delay.h>

#define DELAYTIME 50
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_DDR DDRB


void POVDisplay(uint8_t oneByte)
{
    PORTB = oneByte;
    _delay_ms(2);
}

int main(void) 
{
    //-----------INITS------------//

    uint8_t i = 0x00;
    LED_DDR |= 0xFF;


    //-------EVENT LOOP-----------//
    while(1) 
    {
        while (i < 7)
        {
            //LED_PORT = (1 << i);
            LED_PORT = (1 << i);
            _delay_ms(DELAYTIME);
            i = i + 1;
        }
        while (i > 0)
        {
            LED_PORT = (1 << i);
            _delay_ms(DELAYTIME);
            i = i - 1;
        }
        
    }
    return(0);

}
