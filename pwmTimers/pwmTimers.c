                       /* PWM Demo with serial control over three LEDs */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */

#define LED_DDR DDRB
#define RED_LED PB3
#define GREEN_LED PB2
#define BLUE_LED PB1        

#define RED_CMP OCR2A
#define GREEN_CMP OCR1B
#define BLUE_CMP OCR1A

static inline void initTimers(void) {
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

void setRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    RED_CMP = red;
    GREEN_CMP = green;
    BLUE_CMP = blue;
}
void setRGB_Invert(uint8_t red, uint8_t green, uint8_t blue)
{
    RED_CMP = 255 - red;
    GREEN_CMP = 255 - green;
    BLUE_CMP = 255 - blue;
}
int main(void) {

  int delay = 10;
  uint8_t red_brightness = 0;
  uint8_t green_brightness = 0;
  uint8_t blue_brightness = 0;

  uint8_t sel = 0;
  // -------- Inits --------- //

  initTimers();

  /* enable output on LED pins, triggered by PWM hardware */
  LED_DDR |= (1 << RED_LED);
  LED_DDR |= (1 << GREEN_LED);
  LED_DDR |= (1 << BLUE_LED);
/*
  RED_CMP = red_brightness;
  GREEN_CMP = green_brightness;
  BLUE_CMP = blue_brightness;
*/
  setRGB(red_brightness, green_brightness, blue_brightness);

  _delay_ms(500);
  // ------ Event loop ------ //
  while (1) {
/*
    printString("\r\nEnter (0-255) for PWM duty cycle: ");
    brightness = getNumber();
    OCR2A = OCR1B;
    OCR1B = OCR1A;
    OCR1A = brightness;
*/
/*
    RED_CMP = red_brightness;
    GREEN_CMP = green_brightness;
    BLUE_CMP = blue_brightness;
*/
    setRGB(red_brightness, green_brightness, blue_brightness);
    switch (sel)
    {
        default:
        {   
            red_brightness++;
            green_brightness++;
            blue_brightness++;            
            break;
        }
    }

    for (int i=0; i<delay; i++)
        _delay_ms(1);
  }                                                  /* End event loop */
  return (0);                            /* This line is never reached */
}
