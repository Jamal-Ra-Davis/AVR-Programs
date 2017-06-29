#pragma once

#include <avr/io.h>

#define SPK_REG DDRD
#define SPK_PORT PORTD
#define SPK_PIN (1 << PD6)
#define SPK_PER OCR0A
#define SPK_TOP 20

int spk_cnt;// = 0;
int spk_idx;// = 0;
volatile int spk_periods[2];// = {124, 147};

void initSpeaker();
void setSpeaker(uint8_t level);
void disableSpeaker();
void setSpeakerOsc(int b);
