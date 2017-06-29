
#include <avr/io.h>
#include "MotorDriver.h"

void initMotorDriver()
{
	//Set up motor contol PWM
    DDRB |= (M_STBY | MA_PIN | MB_PIN | MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2);
    MOTOR_PORT &= ~(MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2 | M_STBY);

    TCNT1 = 0;
    OCR1A = 0; //MA Speed
    OCR1B = 0; //MB Speed


	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | FAST_PWM8_1A;
    TCCR1B |= FAST_PWM8_1B | (1 << CS10);

    //TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11) /*| (1 << WGM10)*/;
    //TCCR1B |= /*(1 << WGM13)  | */(1 << WGM12) | (1 << CS10);
}

void setMotorSpeed(int motor, uint16_t speed, int dir)
{
	uint8_t motor_control = MOTOR_PORT;
	switch (motor)
	{
		case MA:
		{
			motor_control &= ~(MA_DIR1 | MA_DIR2);
			switch (dir)
			{
				case FORWARD:
				{
					motor_control |= MA_DIR1;
					MA_SPD = speed;
					//OCR1AL = speed;
                    //OCR1AH = (speed >> 8);
					MOTOR_PORT = motor_control;
					break;
				}
				case REVERSE:
				{
					motor_control |= MA_DIR2;
					MA_SPD = speed;
					//OCR1AL = speed;
					//OCR1AH = (speed >> 8);
					MOTOR_PORT = motor_control;
					break;
				}
				case HARD_BRAKE:
				{
					motor_control |= (MA_DIR1 | MA_DIR2);
					MA_SPD = 0;
					MOTOR_PORT = motor_control;
					break;
				}
				default:
				{
                    MA_SPD = 0;
                    MOTOR_PORT = motor_control;
				}
			}
			break;
		}
		case MB:
		{
			motor_control &= ~(MB_DIR1 | MB_DIR2);
            switch (dir)
            {
                case FORWARD:
                {
                    motor_control |= MB_DIR1;
                    MB_SPD = speed;
                    MOTOR_PORT = motor_control;
                    break;
                }
                case REVERSE:
                {
                    motor_control |= MB_DIR2;
                    MB_SPD = speed;
                    MOTOR_PORT = motor_control;
                    break;
                }
				case HARD_BRAKE:
                {
                    motor_control |= (MB_DIR1 | MB_DIR2);
                    MB_SPD = 0;
                    MOTOR_PORT = motor_control;
                    break;
                }
                default:
                {
                    MB_SPD = 0;
                    MOTOR_PORT = motor_control;
                    break;
                }
            }
			break;
		}
	}
}
void setMotorStandby(int stby)
{
	if (stby)
	{
		MOTOR_PORT |= M_STBY;
	}	
	else
	{
		MOTOR_PORT &= ~M_STBY;
	}
	
}
void motorShutOff()
{
	 //Set up motor contol PWM
    MOTOR_PORT &= ~(MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2 | M_STBY);

    TCNT1 = 0;
    OCR1A = 0; //MA Speed
    OCR1B = 0; //MB Speed
}
