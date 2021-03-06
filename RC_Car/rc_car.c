#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "MotorDriver.h"
#include "Lights.h"
#include "RingBuffer.h"
#include "Speaker.h"

#define BT_RST_PORT PORTD
#define BT_RST_PIN (1 << PD2)

#define BAUD_RATE 9600
#define MYUBRR ((8000000UL/BAUD_RATE)/16 - 1)



uint8_t serialDataAvailable()
{
    return (UCSR0A & (1<<RXC0));
}
void USART_Transmit(unsigned char data)
{
	//Wait for empty tranmit buffer
	while (!(UCSR0A & (1 << UDRE0)));

	UDR0 = data;
}
void sendString(char* str)
{
	int len = strlen(str);
	for (int i=0; i<len; i++)
		USART_Transmit(str[i]);
}
unsigned char USART_Recieve()
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}
void USART_Flush()
{
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}
void readAll()
{
	int cnt = 0;
	int timeout = 1000;
	_delay_ms(10);
	while (1)
	{
		if (serialDataAvailable())
		{
		uint8_t val = USART_Recieve();
		if (val == '\0')
			break;
		}
		cnt += 1;
		if (cnt >= timeout)
			break;
		_delay_ms(10);
	}

}
void sendAT(char *cmd)
{
	sendString(cmd);
	//USART_Recieve();
	_delay_ms(3000);
	//readAll();
}
void setupHM11()
{
	sendAT("AT");
/*
	sendAT("AT+NOTI0");  
  	sendAT("AT+ROLE0");
 	sendAT("AT+RESET");
  	sendAT("AT+SHOW1");
  	sendAT("AT+IMME1");
 	sendAT("AT+NAMEPLZ_WORK");
*/
	sendAT("AT+RENEW");
	
}


RingBuffer rBuf;
ISR(USART_RX_vect) 
{
	uint8_t val = UDR0; 
 	pushRingBuf(&rBuf, val);
}

int main(void) 
{
    //-----------INITS------------//
	clock_prescale_set(clock_div_1);

	initRingBuf(&rBuf);


	while(0);

	//Init serial
	UBRR0H = (unsigned char)(MYUBRR >> 8);
	UBRR0L = (unsigned char)MYUBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0)|(1<<RXCIE0)|(0<<UDRIE0);
	/* Set frame format: 8data, 1stop bit no parity */
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);



	initSpeaker();
	//setSpeaker(1);

	initMotorDriver();
	initLights();

	sei();

	for (int i=0; i<256; i++)
	{
		//OCR2B = i;
		setHeadLights(i);
		_delay_ms(50);
	}

	for (int i=0; i<4; i++)
	{	
		setHeadLights(0x00);
		setSpeaker(0);
		_delay_ms(1000);
	
		setHeadLights(0xFF);
		setSpeaker(1);
		_delay_ms(1000);
	}

	setRearLights(RL0, 1);
	_delay_ms(3000);
	setRearLights(RL0, 0);
	setRearLights(RL1, 1);
	_delay_ms(3000);
	setRearLights(RL0, 1);
	_delay_ms(3000);

	setupHM11();

	

	uint8_t temp;
	setMotorStandby(ENABLE_MOTOR);


	USART_Flush();

	setRearLights(RL0, 1);
	setRearLights(RL1, 1);


	while(0)
	{
		USART_Transmit(0x00);
		_delay_ms(1000);
		USART_Transmit(0x55);
        _delay_ms(1000);
		USART_Transmit(0xAA);
        _delay_ms(1000);
		USART_Transmit(0xFF);
        _delay_ms(1000);
	}
	

    //-------EVENT LOOP-----------//
    while(1) 
    {
		//sendAT("AT");
		//sendString("Hello\n");
		//_delay_ms(5000);
		//continue;
		while(rBuf.cnt > 2)
		{
			//uint8_t value = popRingBuf(&rBuf);
			uint8_t motor = popRingBuf(&rBuf);
			uint8_t spd = popRingBuf(&rBuf);
			uint8_t dir = popRingBuf(&rBuf);
			//setHeadLights(value);
			//USART_Transmit(value);

			setMotorSpeed(motor, spd, dir);
			USART_Transmit(motor);
			USART_Transmit(spd);
			USART_Transmit(dir);
		}
		_delay_ms(500);
		continue;

		if (serialDataAvailable())
		{
			setRearLights(RL0, 1);
			uint8_t value = USART_Recieve();
			setRearLights(RL1, 1);
			setHeadLights(value);
			//USART_Recieve();
			//USART_Recieve();
			//setRearLights(RL0, 0);
            //setRearLights(RL1, 0);		

			if (value == 0x00)	
				while (!rBuf.isEmpty)
				{
					USART_Transmit(popRingBuf(&rBuf));
				}
			else
				pushRingBuf(&rBuf, value);
			USART_Transmit(value);
			USART_Transmit(rBuf.cnt);
        	_delay_ms(1000);
			
			//setRearLights(RL0, 0);
    		//setRearLights(RL1, 0);
		/*
			//PORTD |= (1 << PD6);
			uint8_t value = USART_Recieve();
			OCR2B = value;
			PORTD |= ~(1 << PD6);

			//_delay_ms(10);
			//if (serialDataAvailable())
			//{
				USART_Recieve();
				USART_Recieve();
			//}
		*/	
		}		
		//_delay_ms(80);
		continue;

		//sendString("Hello\n");
		//continue;
		/*
		MOTOR_PORT &= ~(MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2);
		OCR1A = 0;
		temp = MOTOR_PORT;
    	temp &= ~(MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2);
    	temp |= MA_DIR1;
    	MOTOR_PORT = temp;
		for (int i=0; i<512; i+= 50)
		{
			TCNT1 = 0;
			OCR1A = i;
			_delay_ms(3000);
		}
		*/
		PORTD &= ~((1 << PD4) | (1 << PD5));
		OCR2B = 255;
		setMotorSpeed(MA, 0, SOFT_BRAKE);
		setMotorSpeed(MB, 0, SOFT_BRAKE);
		_delay_ms(10000);
		OCR2B = 0;
		PORTD |= (1 << PD4) | (1 << PD5);
		for (int i=0; i<255; i+=25)
		{
			if (i > 255)
				OCR2B = 100;		
			setMotorSpeed(MA, i, FORWARD);
			_delay_ms(3000);
		}
		PORTD &= ~((1 << PD4) | (1 << PD5));

		OCR2B = 255;
        setMotorSpeed(MA, 0, SOFT_BRAKE);
        setMotorSpeed(MB, 0, SOFT_BRAKE);
        _delay_ms(10000);
        OCR2B = 0;
        for (int i=0; i<255; i+=25)
        {
            if (i > 255)
                OCR2B = 100;
            setMotorSpeed(MA, i, REVERSE);
            _delay_ms(3000);
        }


		OCR2B = 255;
        setMotorSpeed(MA, 0, SOFT_BRAKE);
        setMotorSpeed(MB, 0, SOFT_BRAKE);
        _delay_ms(10000);
        OCR2B = 0;
        for (int i=0; i<255; i+=25)
        {
            if (i > 255)
                OCR2B = 100;
            setMotorSpeed(MB, i, FORWARD);
            _delay_ms(3000);
        }

		OCR2B = 255;
        setMotorSpeed(MA, 0, SOFT_BRAKE);
        setMotorSpeed(MB, 0, SOFT_BRAKE);
        _delay_ms(10000);
        OCR2B = 0;
        for (int i=0; i<255; i+=25)
        {
            if (i > 255)
                OCR2B = 100;
            setMotorSpeed(MB, i, REVERSE);
            _delay_ms(3000);
        }
        continue;


		//sendString("Hi\n");
		MOTOR_PORT &= ~(MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2);
		OCR1A = 0;
		temp = MOTOR_PORT;
        temp &= ~(MA_DIR1 | MA_DIR2);
        temp |= MA_DIR2;
        MOTOR_PORT = temp;
		for (int i=0; i<512; i+= 50)
        {
            TCNT1 = 0;
            OCR1A = i;
            _delay_ms(3000);
        }


		//sendString("Greetings\n");
		OCR1A = 0;
		temp = MOTOR_PORT;
        temp &= ~(MA_DIR1 | MA_DIR2 | MB_DIR1 | MB_DIR2);
        temp |= MB_DIR1;
        MOTOR_PORT = temp;
		for (int i=0; i<512; i+= 50)
        {
            TCNT1 = 0;
            OCR1B = i;
            _delay_ms(3000);
        }

		//sendString("Sup\n");
		OCR1A = 0;
        temp = MOTOR_PORT;
        temp &= ~(MB_DIR1 | MB_DIR2);
        temp |= MB_DIR2;
        MOTOR_PORT = temp;
		for (int i=0; i<512; i+= 50)
        {
            TCNT1 = 0;
            OCR1B = i;
            _delay_ms(3000);
        }
    }
    return(0);

}
