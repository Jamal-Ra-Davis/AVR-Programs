#include <avr/io.h>
#include "ColorDriver.h"
#include "delay.h"

void initTimers(void) {
  // Timer 1 A,B
  TCCR1A |= (1 << WGM10);                      /* Fast PWM mode, 8-bit */
  TCCR1B |= (1 << WGM12);                       /* Fast PWM mode, pt.2 */
  TCCR1B |= (1 << CS11);                     /* PWM Freq = F_CPU/8/256 */
  TCCR1A |= (1 << COM1A1);                      /* PWM output on OCR1A */
  TCCR1A |= (1 << COM1B1);                      /* PWM output on OCR1B */

  TCCR1A |= (1 << COM1A1)|(1 << COM1A0);//Non inverting mode, A
  TCCR1A |= (1 << COM1B1)|(1 << COM1B0);//Non inverting mode, B

  // Timer 2
  TCCR2A |= (1 << WGM20);                             /* Fast PWM mode */
  TCCR2A |= (1 << WGM21);                       /* Fast PWM mode, pt.2 */
  TCCR2B |= (1 << CS21);                     /* PWM Freq = F_CPU/8/256 */
  TCCR2A |= (1 << COM2A1);                      /* PWM output on OCR2A */

  TCCR2A |= (1 << COM2A1)|(1 << COM2A0);//Non inverting mode
}

void initColorDriver(ColorDriver *colorDriver, uint8_t r, uint8_t g, uint8_t b)
{
    initTimers();
    colorDriver->inverting = 0;
    
    colorDriver->red = r;
    colorDriver->green = g;
    colorDriver->blue = b;
}
void setNonInverting()
{
    TCCR1A |= (1 << COM1A1)|(1 << COM1A0);
    TCCR1A |= (1 << COM1B1)|(1 << COM1B0);
    
    TCCR2A |= (1 << COM2A1)|(1 << COM2A0);
}
void setInverting()
{
    TCCR1A |= (1 << COM1A1);
    TCCR1A &= ~(1 << COM1A0);

    TCCR1A |= (1 << COM1B1);
    TCCR1A &= ~(1 << COM1B0);

    TCCR2A |= (1 << COM2A1);
    TCCR2A &= ~(1 << COM2A0);
}
void setRGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t inverting)
{
    if (!inverting)
    {
        RED_CMP = red;
        GREEN_CMP = green;
        BLUE_CMP = blue;
    }
    else
    {
        RED_CMP = 255 - red;
        GREEN_CMP = 255 - green;
        BLUE_CMP = 255 - blue;
    }
}
void setRGB_NI(uint8_t red, uint8_t green, uint8_t blue)
{
    setRGB(red, green, blue, 0);
}
void setRGB_driver(ColorDriver *colorDriver)
{
    if (!colorDriver->inverting)
    {
        RED_CMP = colorDriver->red;
        GREEN_CMP = colorDriver->green;
        BLUE_CMP = colorDriver->blue;
    }
    else
    {
        RED_CMP = 255 - colorDriver->red;
        GREEN_CMP = 255 - colorDriver->green;
        BLUE_CMP = 255 - colorDriver->blue;
    }
}
void enableOutputLeds()
{
    LED_DDR |= (1 << RED_LED);
    LED_DDR |= (1 << GREEN_LED);
    LED_DDR |= (1 << BLUE_LED);
}
ColorDriver transitionColor(uint8_t red_curr, uint8_t green_curr, uint8_t blue_curr, uint8_t red_new, uint8_t green_new, uint8_t blue_new, int delay)
{
    int redDiff = red_new - red_curr;
    int greenDiff = green_new - green_curr;
    int blueDiff = blue_new - blue_curr;

    while (1)
    {
        if ((red_curr == red_new)&&(green_curr == green_new)&&(blue_curr == blue_new))
            break;

        if (red_curr != red_new)
        {
            if (redDiff > 0)
                red_curr++;
            else
                red_curr--;
        }
        if (blue_curr != blue_new)
        {
            if (blueDiff > 0)
                blue_curr++;
            else
                blue_curr--;
        }
        if (green_curr != green_new)
        {
            if (greenDiff > 0)
                green_curr++;
            else
                green_curr--;
        }

        setRGB_NI(red_curr, green_curr, blue_curr);
        delay_ms(delay);
    }

    ColorDriver outColors;
    outColors.red = red_curr;
    outColors.green = green_curr;
    outColors.blue = blue_curr;
    return outColors;
}
uint8_t updateColor(uint8_t *red_curr, uint8_t *green_curr, uint8_t *blue_curr, uint8_t red_new, uint8_t green_new, uint8_t blue_new, int delay)
{
    int redDiff = red_new - *red_curr;
    int greenDiff = green_new - *green_curr;
    int blueDiff = blue_new - *blue_curr;

    if ((*red_curr == red_new)&&(*green_curr == green_new)&&(*blue_curr == blue_new))
        return 1;

    if (*red_curr != red_new)
    {
        if (redDiff > 0)
            (*red_curr)++;
        else
            (*red_curr)--;
    }
    if (*blue_curr != blue_new)
    {
        if (blueDiff > 0)
            (*blue_curr)++;
        else
            (*blue_curr)--;
    }
    if (*green_curr != green_new)
    {
        if (greenDiff > 0)
            (*green_curr)++;
        else
            (*green_curr)--;
    }
    setRGB_NI(*red_curr, *green_curr, *blue_curr);
    delay_ms(delay);

    return 0;
}

/*
void setRGB_Invert(uint8_t red, uint8_t green, uint8_t blue)
{
    RED_CMP = 255 - red;
    GREEN_CMP = 255 - green;
    BLUE_CMP = 255 - blue;
}
*/
