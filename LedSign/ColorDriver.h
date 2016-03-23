#pragma once
#include <avr/io.h>

#define LED_DDR DDRB
#define RED_LED PB3
#define GREEN_LED PB2
#define BLUE_LED PB1        

#define RED_CMP OCR2A
#define GREEN_CMP OCR1B
#define BLUE_CMP OCR1A

typedef struct
{
    uint8_t red, green, blue;
    uint8_t inverting;
} ColorDriver;

void initTimers(void); 

void initColorDriver(ColorDriver *colorDriver, uint8_t r, uint8_t g, uint8_t b);
void setNonInverting();
void setInverting();
void setRGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t inverting);
void setRGB_NI(uint8_t red, uint8_t green, uint8_t blue);
void setRGB_driver(ColorDriver *colorDriver);

void enableOutputLeds();

ColorDriver transitionColor(uint8_t red_curr, uint8_t green_curr, uint8_t blue_curr, uint8_t red_new, uint8_t green_new, uint8_t blue_new, int delay);
uint8_t updateColor(uint8_t *red_curr, uint8_t *green_curr, uint8_t *blue_curr, uint8_t red_new, uint8_t green_new, uint8_t blue_new, int delay);

/*
void setRGB_Invert(uint8_t red, uint8_t green, uint8_t blue)
{
    RED_CMP = 255 - red;
    GREEN_CMP = 255 - green;
    BLUE_CMP = 255 - blue;
}
*/
