#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"


int main(void) 
{
    char serialCharacter;
    //-----------INITS------------//
    LED_DDR = 0xFF;
    initUSART();
    printString("Hello World!\r\n");


    //-------EVENT LOOP-----------//
    while(1) 
    {
        serialCharacter = receiveByte();
        transmitByte(serialCharacter);
        LED_PORT = serialCharacter;
    }
    return(0);

}
