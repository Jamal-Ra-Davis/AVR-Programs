#pragma once
#include <avr/io.h>

#define GND_PORTS PORTD
#define LEDS_PORT PORTC

typedef struct
{
    uint8_t regClk, latchClk, dataPin;
} LedDriver;

LedDriver* makeLedDriver(uint8_t, uint8_t, uint8_t);
void setLedDriver(LedDriver*, uint8_t, uint8_t, uint8_t);


void sr_loadByte(LedDriver* ledDriver, uint8_t data);
void sr_loadData_old(LedDriver* ledDriver, volatile uint8_t* data, int numBits);//Old function, want to reverse order of load
void sr_loadData(LedDriver* ledDriver, volatile uint8_t* data, int numBits);


