#include <avr/io.h>
#include <avr/interrupt.h>

#include "Speaker.h"

//int spk_cnt = 0;
//int spk_idx = 0;
//int spk_periods[2] = {124, 147};

void initSpeaker()
{
	spk_cnt = 0;
	spk_idx = 0;
	spk_periods[0] = 124;
	spk_periods[1] = 147;

	TCNT0 = 0;
    SPK_PER = spk_periods[spk_idx];
    TCCR0A |= (1 << COM0A0) | (1 << WGM01);
    TCCR0B |= (1 << CS01) | (1 << CS00);

	SPK_REG |= SPK_PIN;

	TIMSK2 |= (1 << TOIE2);
}
void setSpeaker(uint8_t level)
{
	if (level)
		SPK_REG |= SPK_PIN;
	else
		SPK_REG &= ~SPK_PIN;
}
void disableSpeaker()
{
	//setSpeaker(0);
	SPK_REG &= ~SPK_PIN;
	setSpeakerOsc(0);
	TIMSK2 &= ~(1 << TOIE2);
}
void setSpeakerOsc(int b)
{
    if (b)
    {
        TCCR0A |= (1 << COM0A0);
    }
    else
    {
        TCCR0A &= ~(1 << COM0A0);
    }
}
ISR(TIMER2_OVF_vect)
{
    spk_cnt++;
    if (spk_cnt >= SPK_TOP)
    {
        spk_cnt = 0;
        spk_idx ^= 1;
        SPK_PER = spk_periods[spk_idx];
		//PORTD ^= (1 << PD3);
    }
}
