#pragma once
#include <avr/io.h>


void setPinmode(uint8_t pin, uint8_t direction);
void digitalWrite(uint8_t pin, uint8_t val);

