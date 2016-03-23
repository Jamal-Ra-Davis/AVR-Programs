#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/interrupt.h>
#include "ColorDriver.h"
#include "delay.h"
#include "USART.h"


volatile uint8_t state = 2;
volatile char rgb[3] = {0, 0, 0};
volatile uint8_t delay = 25;
volatile uint8_t delay_anm = 25;

uint8_t serialDataAvailable()
{
    return (UCSR0A & (1<<RXC0));
}

ISR(USART_RX_vect)
{
    char command = receiveByte();

    switch (command)
    {
        case 'S'://Set and Hold specific color
        {
            char rgb_[3];
            uint8_t transition = 0;
            uint8_t delay_ = 0;

            //Get RGB values
            for (int i=0; i<3; i++)
                rgb_[i] = receiveByte();

            //Get whether or not transition is instantaneous or not
            transition = receiveByte();
            if ((transition != 0)&&(transition != 1))
                break;

            if (!transition)
            {
                state = 0;
                for (int i=0; i<3; i++)
                    rgb[i] = rgb_[i];
            }
            else
            {
                //Get transition delay value
                delay_ = receiveByte();

                state = 1;
                for (int i=0; i<3; i++)
                    rgb[i] = rgb_[i];
                delay = delay_;
            }
            break;
        }
        case 'A'://Swtich to transition animation and set speed
        {
            uint8_t delay_;

            //Get transition delay value
            delay_ = receiveByte();

            state = 2;
            delay_anm = delay_;
            break;
        }
    }

    while (serialDataAvailable())
    {
        receiveByte();
        _delay_us(100);
    }

}


int main(void) {

    ColorDriver colorDriver;
  
    uint8_t red_brightness = 255;
    uint8_t green_brightness = 0;
    uint8_t blue_brightness = 0;
    uint8_t sel = 0;

    // -------- Inits --------- //
    initUSART(); 
    UCSR0B |= (1 << RXCIE0);

    while (serialDataAvailable())
        receiveByte();

    sei();

    initColorDriver(&colorDriver, 0, 0, 0);
    setRGB_NI(red_brightness, green_brightness, blue_brightness);
    enableOutputLeds();
  
    _delay_ms(500);

    // ------ Event loop ------ //
    while (1) 
    {
        switch (state)
        {
            case 0:
            {
                red_brightness = rgb[0];
                green_brightness = rgb[1];
                blue_brightness = rgb[2];

                setRGB_NI(red_brightness, green_brightness, blue_brightness);
                state = 255;
                break;
            }
            case 1:
            {
                updateColor(&red_brightness, &green_brightness, &blue_brightness, rgb[0], rgb[1], rgb[2], delay);
                break;
            }
            case 2:
            {
                if (sel == 0)
                {
                    if (updateColor(&red_brightness, &green_brightness, &blue_brightness, 255, 0, 0, delay_anm))
                        sel = 1;
                }
                else if (sel == 1)
                {
                    if (updateColor(&red_brightness, &green_brightness, &blue_brightness, 0, 255, 0, delay_anm))
                        sel = 2;
                }
                else if (sel == 2)
                {
                    if (updateColor(&red_brightness, &green_brightness, &blue_brightness, 0, 0, 255, delay_anm))
                        sel = 0;
                }
                break;
            }
        }
  }                                                  
  return (0);                            
}
