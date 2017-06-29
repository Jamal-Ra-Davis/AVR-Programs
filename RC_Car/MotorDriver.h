#pragma once

#include <avr/io.h>

#define FAST_PWM8_1A (1 << WGM10)
#define FAST_PWM8_1B (1 << WGM12)

#define FAST_PWM9_1A (1 << WGM11)
#define FAST_PWM9_1B (1 << WGM12)

#define FAST_PWM10_1A (1 << WGM11)|(1 << WGM10) 
#define FAST_PWM10_1B (1 << WGM12)


#define MOTOR_PORT PORTB
#define M_STBY (1 << PB0)
#define MA_PIN (1 << PB1)
#define MB_PIN (1 << PB2)
#define MA_DIR1 (1 << PB3)
#define MA_DIR2 (1 << PB4)
#define MB_DIR1 (1 << PB5)
#define MB_DIR2 (1 <<PB6)
#define MA_SPD OCR1A
#define MB_SPD OCR1B

#define MA 0
#define MB 1
#define FORWARD 0
#define REVERSE 1
#define HARD_BRAKE 2
#define SOFT_BRAKE 3

#define STBY_OFF 0
#define STBY_ON 1

#define ENABLE_MOTOR 1
#define DISABLE_MOTOR 0

void initMotorDriver();
void setMotorSpeed(int motor, uint16_t speed, int dir);
void setMotorStandby(int stby);
void motorShutOff();
