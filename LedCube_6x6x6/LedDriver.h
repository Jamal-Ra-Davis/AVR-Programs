#pragma once
#include <avr/io.h>
#include "FrameBuffer.h"

#define GND_DDR DDRD
#define GND_PORTS PORTD

#define GND_EN_DDR DDRB
#define GND_EN_PORT PORTB
#define GND_EN_PIN PB1

#define SR_PORT PORTC

typedef struct
{
    uint8_t regClk, latchClk, redData, greenData, blueData;
} LedDriver;
typedef struct
{
    uint8_t regClk, latchClk, data;
} GroundDriver;



LedDriver* makeLedDriver(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void setLedDriver(LedDriver*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

GroundDriver* makeGroundDriver(uint8_t, uint8_t, uint8_t);
void setGroundDriver(GroundDriver*, uint8_t, uint8_t, uint8_t);
void enableGroundDriver(GroundDriver*);
void disableGroundDriver(GroundDriver*);

void sr_loadByte(LedDriver* ledDriver, uint8_t data);
void sr_loadData_old(LedDriver* ledDriver, volatile uint8_t* data, int numBits);//Old function, want to reverse order of load
void sr_loadData(LedDriver* ledDriver, volatile uint8_t* data, int numBits);
void sr_loadData_alt(LedDriver* ledDriver, volatile uint8_t data[3][FBUF_REGS], int numBits);

void initGroundRegister(GroundDriver* groundDriver);
void sr_incrementGround(GroundDriver* groundDriver, FrameBuffer* framebuffer);
