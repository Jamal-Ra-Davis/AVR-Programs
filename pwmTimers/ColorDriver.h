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
} ColorDriver;

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

void initColorDriver(
