#pragma once

#include <avr/io.h>

#define HL_REG DDRD
#define HL_PORT PORTD
#define HL_PIN (1 << PD3)
#define HL_BRIGHTNESS OCR2B

#define RL_REG DDRD
#define RL_PORT PORTD
#define RL_PIN0 (1 << PD4)
#define RL_PIN1 (1 << PD5)
#define RL0 0
#define RL1 1


void initLights();
void setHeadLights(uint8_t brightness);
void setRearLights(int pin, int level);
void disableLights();
