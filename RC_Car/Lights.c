
#include <avr/io.h>
#include "Lights.h"

void initLights()
{
	TCNT2 = 0;
    HL_BRIGHTNESS = 0;//255;
    TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B |= (0 << WGM22) | (0 << CS22) | (1 << CS21) | (0 << CS20);

    HL_REG |= HL_PIN;
	RL_REG |= (RL_PIN0 | RL_PIN1);

	RL_PORT &= ~(RL_PIN0 | RL_PIN1);
}
void setHeadLights(uint8_t brightness)
{
	if (!brightness)
		HL_REG &= ~HL_PIN;
	else
		HL_REG |= HL_PIN;

	HL_BRIGHTNESS = brightness;
}
void setRearLights(int pin, int level)
{
	switch (pin)
	{
		case RL0:
		{
			if (level)
				RL_PORT |= RL_PIN0; 
			else
				RL_PORT &= ~RL_PIN0;		
			break;
		}
		case RL1:	
			if (level)
                RL_PORT |= RL_PIN1; 
            else
                RL_PORT &= ~RL_PIN1;
			break;
	}
}
void disableLights()
{
	HL_REG &= ~HL_PIN;
    RL_REG &= ~(RL_PIN0 | RL_PIN1);
}
