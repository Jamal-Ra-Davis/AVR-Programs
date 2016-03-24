//#define F_CPU 8000000


#include <stdlib.h>
#include <avr/power.h>
#include <util/setbaud.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "FrameBuffer.h"
#include "LedDriver.h"
#include "Vector3d.h"
#include "ButtonController.h"
#include "Tritris.h"
#include "delay.h"
#include "Character.h"
#include "RingBuffer.h"
#include "USART.h"
#include "MessageHandler.h"

#define LEDS_DDR DDRC
//#define GND_DDR DDRD

#define REG_CLK PC1//flip reg clk and latch clk in final code
#define LATCH_CLK PC2//flip reg clk and latch clk in final code
#define RED_DATA PC3
#define GREEN_DATA PC4
#define BLUE_DATA PC5


//#define TIMER_DELAY 21 //45 Hz
//#define TIMER_DELAY 16 //60 Hz, delay 60
//#define TIMER_DELAY 255 //120 Hz, delay 30


#define TIMER_DELAY 60//60 for ~90Hz 80 for ~60

#define DELAY_TIME 45

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((8000000 / (USART_BAUDRATE * 16UL))) - 1)



volatile uint16_t timer_val;
volatile FrameBuffer framebuffer;
volatile LedDriver ledDriver;
volatile GroundDriver groundDriver;
volatile RingBuffer ringbuffer;
ButtonController buttonController;


void USARTInit()
{
    /*Set baud rate */
    UBRR0H = (BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    /*Enable receiver and transmitter and Receive interupt and Data Register Empty*/
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<UDRIE0);

    /* Set frame format: 8data, 1stop bit no parity */
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void setSerialRecieveInterrupt(uint8_t setting)
{
    if (setting)
        UCSR0B |= (1 << RXCIE0);
    else
        UCSR0B &= ~(1 << RXCIE0);
}
uint8_t serialDataAvailable()
{
    return (UCSR0A & (1<<RXC0));
}

ISR(USART_RX_vect)
{
    while (serialDataAvailable())
    {
        pushRingBuf(&ringbuffer, receiveByte());
        _delay_us(800);
    }
}

void initTimer()
{
    //init timers
    cli();              //disable global interrupts
    TCCR0A = 0;         //Clear TTR1 registers
    TCCR0B = 0;

    //Enable timer1 compareA interrupt
    TIMSK0 = (1 << OCIE0A);

    
    //TCCR0B |= ((1 << CS01) | (0 << CS00));// 1/8 clock speed
    //TCCR0B |= ((1 << CS02) | (0 << CS01) | (1 << CS00));// 1/1024
    //TCCR0B |= ((1 << CS01) | (1 << CS00));// 1/64 clock speed
    TCCR0B |= ((1 << CS02) | (0 << CS01) | (0 << CS00));// 1/256 clock speed    <<-- The one you want

    // turn on CTC mode:
    TCCR0A |= (1 << WGM01);

    OCR0A = TIMER_DELAY;//21 is a good value
    TCNT0 = 0;   

    //Set 2nd timer
    //TCCR1B |= (1 << CS11)|(0 << CS10);// Set at 1/8th clock speed
    //TCCR1B |= (1 << CS10); //No Scaling 1 MHz
    TCCR1B |= (1 << CS11); // 1/8 clock speed 

    sei();
}

ISR(TIMER0_COMPA_vect)
{
//    GND_PORTS &= 0xF0;
    GND_EN_PORT |= (1 << GND_EN_PIN);
    //TCNT1 = 0;//--
    sr_loadData_alt(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel], 36);

//    GND_PORTS |= 1 << framebuffer.currLevel;
/*
    framebuffer.currLevel++;
    if (framebuffer.currLevel == FBUF_SZ)
        framebuffer.currLevel = 0;
*/
    sr_incrementGround(&groundDriver, &framebuffer);
    GND_EN_PORT &= ~(1 << GND_EN_PIN);
}

int main(void) 
{
    clock_prescale_set(clock_div_1);
    //-----------INITS------------//
    LEDS_DDR |= 0xFF;

    GND_DDR = 0x0F;
    GND_PORTS = 0x00;

    

    initFrameBuffer(&framebuffer);
    setLedDriver(&ledDriver, REG_CLK, LATCH_CLK, RED_DATA, GREEN_DATA, BLUE_DATA);

    setGroundDriver(&groundDriver, PD3, PD4, PD2);
    initGroundRegister(&groundDriver);    

    initButtonController(&buttonController);

    setFrameBuffer(&framebuffer);
    //clearFrameBuffer_(&framebuffer);
    frameBufferToBits_DB_NEW(&framebuffer);
    //sr_loadData_alt(&ledDriver, framebuffer.bufferBits[0], 36);

    
    initRingBuf(&ringbuffer);
    USARTInit();
    initTimer();//Global interrupts are enabled in initTimer
    //pushRingBuf(&ringbuffer, 1);
    //pushRingBuf(&ringbuffer, 2);
    //pushRingBuf(&ringbuffer, 3);
    //pushRingBuf(&ringbuffer, 4);



    _delay_ms(10000); 
    setFrameBufferColor(&framebuffer, 7);
    frameBufferToBits_DB_NEW(&framebuffer);    

    //uint8_t num = 0;
    //[layers][color][data]
    /*
    uint8_t data[4][3][5] = {
                                {
                                    {0x08, 0xFF, 0xFF, 0xFF, 0xFF},
                                    //{0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                    //{0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
                                    {0x00, 0x00, 0x00, 0x00, 0x00},
                                    {0x00, 0x00, 0x00, 0x00, 0x00}
                                },
                                {
                                    {0x00, 0x00, 0x00, 0x00, 0x00},
                                    {0x00, 0x00, 0x00, 0x00, 0x00},
                                    {0x00, 0x00, 0x00, 0x00, 0x00}
                                },
                                {
                                    {0x00, 0x00, 0x00, 0x00, 0xFF},
                                    {0x00, 0x00, 0x00, 0x00, 0xFF},
                                    {0x00, 0x00, 0x00, 0x00, 0xFF}
                                },
                                {
                                    {0x00, 0x00, 0x00, 0x00, 0x00},
                                    {0x00, 0x00, 0x00, 0x00, 0x00},
                                    {0x00, 0x00, 0x00, 0x00, 0x00},
                                }
                            };

    */
    
    DDRD = 0xFF;
    PORTD = 0x00;
/*
    uint8_t byte = 0xFF;
    uint8_t mask;
    for (int i=7; i>=0; i--)
    {
        PORTD &= ~(1 << PD3);//Triggers on rising edge of clock, must bring low first

        mask = 1 << i;
        if (mask & byte)
            SR_PORT |= (1 << PD2);
        else
            SR_PORT &= ~(1 << PD2);

        SR_PORT |= (1 <<PD3);//Loads data from data pin into shift reg on rising edge
    }
    SR_PORT &= ~(1 << PD4);
    SR_PORT |= (1 << PD4);
*/ 
/* 
    clearFrameBuffer_(&framebuffer);
    for (int i=0; i<FBUF_SZ; i++)
    {
        drawPlane(&framebuffer, 2, i, 7);
        clearPixel(&framebuffer, 0, 4, i);
    }
    frameBufferToBits_DB_NEW(&framebuffer);
    cli();
    while (1)
    {
        GND_EN_PORT |= (1 << GND_EN_PIN);
        sr_loadData_alt(&ledDriver, framebuffer.bufferBits[framebuffer.currLevel], 36);
        sr_incrementGround(&groundDriver, &framebuffer);
        GND_EN_PORT &= ~(1 << GND_EN_PIN);
        _delay_ms(15000);
    }
*/

    while (0)
    {
/*
        Vector3d start;
        setVector3d(&start, 0, 0, 0);
        Vector3d end;
        for (end.z = 0; end.z<FBUF_SZ; end.z++)
        {
            for (end.x = 0; end.x < FBUF_SZ; end.x++)
            {
                for (end.y = 0; end.y < FBUF_SZ; end.y++)
                {
                    clearFrameBuffer_(&framebuffer);
                    drawLine3d(&framebuffer, &start, &end, WHITE);
                    frameBufferToBits_DB_NEW(&framebuffer);
                    _delay_ms(250);
                }
            }
        }
*/
/*
        for (int i=0; i<FBUF_SZ; i++)
        {
            clearFrameBuffer_(&framebuffer);
            drawLine2d(&framebuffer, 2, FBUF_SZ-1, 0, i, FBUF_SZ-1, FBUF_SZ-1-i, WHITE);
            //setPixelColor(&framebuffer, 0, i, 0, RED);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(500);
        }
        for (int i=1; i<FBUF_SZ-1; i++)
        {
            clearFrameBuffer_(&framebuffer);
            drawLine2d(&framebuffer, 2, FBUF_SZ-1, i, FBUF_SZ-1, FBUF_SZ-1-i, 0, WHITE);
            //setPixelColor(&framebuffer, i, FBUF_SZ-1, 0, RED);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(500);
        }
*/
        rotatePlane_anm(&framebuffer, rand()%2, RANDOM_COLOR, 250);

    }


    while (0)
    { 
/*
        clearFrameBuffer_(&framebuffer);
        drawPlane(&framebuffer, 2, 0, 7);
        //setPixelColor(&framebuffer, 0, 4, 0, 2);
        frameBufferToBits_DB_NEW(&framebuffer);
        //clearFrameBuffer_(&framebuffer);
        //drawPlane(&framebuffer, 2, 0, 4);
        //frameBufferToBits_DB_NEW(&framebuffer); 
*/ 
       /*
        clearFrameBuffer_(&framebuffer);
        drawPlane(&framebuffer, 2, 0, 7);
        frameBufferToBits_DB_NEW(&framebuffer);
        _delay_ms(5000);
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                clearFrameBuffer_(&framebuffer);
                drawPlane(&framebuffer, 2, 0, 7);
                clearPixel(&framebuffer, i, j, 0);
                frameBufferToBits_DB_NEW(&framebuffer);
                _delay_ms(5000);
            }
        }
        */
        //_delay_ms(500);    
       //drawPlane(&framebuffer, 2, 0, 4); 
        Vector3d movement;
        int x, y, z;
        while(1)
        {
            x = rand()%3-1;
            y = rand()%3-1;
            z = rand()%3-1;
            if (!(x == 0 && y == 0 && z == 0))
                break;
        }
        setVector3d(&movement, x, y, z);
        boxExpand_anm(&framebuffer, &movement, rand()%7+1, 100); 
        //snake_anm(&framebuffer, 50, rand()%7+1, 500); 
    }



    //Testing serial stuff, remember to get rid of
    char serialWords[97] = "TESTING TESTING 1234 567 89";//"DEFAULT TEXT IS COOL";
    //char serialWords[129] = "TEST MESSAGE THAT NEEDS TO BE 128 CHARACTERS LONG  THE OTHER MESSAGE WASNT LONG ENOUGH SO I AM ADDING TO IT 123456789 12345 1234";
    while(1)
    {
        int serialCnt = 0;
/*
        for (int i=0; i<8; i++)
        {
            clearFrameBuffer_(&framebuffer);    
            setFrameBufferColor(&framebuffer, i);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(1000);
        }
*/
        //sprintf(serialWords, "DEFAULT TEXT");

        int buffer_used = 0;
        while (!ringbuffer.isEmpty)
        {
            buffer_used = 1;
            char val = popRingBuf(&ringbuffer);
            serialWords[serialCnt] = val; 
            //transmitByte(val);
            serialCnt++;
            //setFrameBufferColor(&framebuffer, val);
            //frameBufferToBits_DB_NEW(&framebuffer);
            //_delay_ms(500);
        }
        if (buffer_used)
            serialWords[serialCnt] = '\0';


        int anm_num = 9;
        //int rand_anm = rand()%anm_num;
        int rand_anm = 0;
        int rand_loops;
        int rand_delay;
        switch (rand_anm)
        {
            case 0:
            {
                rand_loops = rand()%5+1;
                for (int l=0; l<rand_loops; l++)
                {
                    Marquee serialMarquee;

                    initMarquee(&serialMarquee, 0, serialWords, WHITE);
                    for (int i=0; i<serialMarquee.text_size; i++)
                    {
                        serialMarquee.text[i].color = RANDOM_COLOR;
                    }
                    for (serialMarquee.offset = 0; serialMarquee.offset < marqueeEndIdx(&serialMarquee); serialMarquee.offset++)
                    {
                        clearFrameBuffer_(&framebuffer);
                        drawMarquee_alt(&framebuffer, &serialMarquee);
                        frameBufferToBits_DB_NEW(&framebuffer);
                        _delay_ms(500);
                    }
                }
                break;
            }
            case 1:
            {
                rand_loops = rand()%15+15;
                rand_delay = (rand()%4+1)*100;
                for (int l=0; l<rand_loops; l++)
                {
                    shiftPlane_anm(&framebuffer, rand()%3, RANDOM_COLOR, rand_delay);
                }
                break;
            }
            case 2:
            {   
                rand_loops = rand()%15+15;
                rand_delay = (rand()%4+1)*100;
                for (int l=0; l<rand_loops; l++)
                {
                    rotatePlane_anm(&framebuffer, rand()%3, RANDOM_COLOR, rand_delay);
                }
                break;
            }
            case 3:
            {
                rand_loops = (rand()%4+1)*50;
                rand_delay = (rand()%4+1)*100;
                snake_anm(&framebuffer, rand_loops, RANDOM_COLOR, rand_delay);
                break;
            }
            case 4:
            {
                rand_loops = rand()%40+25;
                rand_delay = (rand()%4+2)*100;
                for (int l=0; l<rand_loops; l++)
                {
                    setFrameBufferRandom(&framebuffer);
                    frameBufferToBits_DB_NEW(&framebuffer);
                    delay_ms(rand_delay);
                    //ballBounce_anm(&framebuffer, rand_delay);
                }
                break;
            }
            case 5:
            {
                rand_loops = rand()%50+10;
                rand_delay = (rand()%10+8)*10;
                for (int l=0; l<rand_loops; l++)
                {
                    Vector3d movement;
                    int x, y, z;
                    while(1)    
                    {
                        x = rand()%3-1;
                        y = rand()%3-1;
                        z = rand()%3-1;
                        if (!(x == 0 && y == 0 && z == 0))
                            break;
                    }
                    setVector3d(&movement, x, y, z);
                    boxExpand_anm(&framebuffer, &movement, RANDOM_COLOR, rand_delay);//100 as delay value, don't stray too far
                }
                break;
            }
            case 6:
            {
                rand_loops = rand()%40+25;
                rand_delay = (rand()%4+2)*100;
                for (int l=0; l<rand_loops; l++)
                {
                    clearFrameBuffer_(&framebuffer);
                    for (int i=0; i<rand()%10+5; i++)
                    {
                        setPixelColor(&framebuffer, rand()%FBUF_SZ, rand()%FBUF_SZ, rand()%FBUF_SZ, RANDOM_COLOR);
                    }
                    frameBufferToBits_DB_NEW(&framebuffer);
                    delay_ms(rand_delay);
                    //ballBounce_anm(&framebuffer, rand_delay);
                }
                break;
            }
            case 7:
            {       
                rand_loops = rand()%15+15;
                rand_delay = (rand()%10+8)*20;
                uint8_t color_ = RANDOM_COLOR;
                for (int l=0; l<rand_loops; l++)
                {
                    pointCloud_anm(&framebuffer, 0, RANDOM_COLOR, rand_delay);
                }
                break;
            }
            case 8:
            {
                rand_delay = (rand()%11)*100 + 250;
                snakeGame(&framebuffer, 1, rand_delay);
                break;
            }
        }
    }


   
    clearFrameBuffer_(&framebuffer); 
    frameBufferToBits_DB_NEW(&framebuffer); 
    MessageHandler messageHandler;
    initMessageHandler(&messageHandler, &ringbuffer);

    while(0)
    {
        //clearFrameBuffer_(&framebuffer);        
        //frameBufferToBits_DB_NEW(&framebuffer);

        //uint8_t mode = 'Q';
        //uint8_t anm_mode = 0;
        while (!ringbuffer.isEmpty)
        {
            /*
            setFrameBufferColor(&framebuffer, WHITE);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(500);        
            */
            uint8_t valid_message = 1;
            switch(popRingBuf(&ringbuffer))
            {
                case 'A':
                {
                    if (handleAnimationMessage(&messageHandler, &framebuffer))
                        valid_message = 0;
                    break;
                }
                case 'C':
                {
                    break;
                }
                case 'D':
                {
                    //setFrameBufferColor(&framebuffer, RED);
                    //frameBufferToBits_DB_NEW(&framebuffer);
                    //_delay_ms(500);
                    if (handleDrawingMessage(&messageHandler, &framebuffer))
                        valid_message = 0;
                    //clearFrameBuffer_(&framebuffer);        
                    //frameBufferToBits_DB_NEW(&framebuffer);
                    break;
                }
                case 'M':
                {
                    break;
                }
                case 'Q':
                {
                    break;
                }
                default:
                {
                    //Failed
                    valid_message = 0;
                    break;
                }
            }
            if (!valid_message)
            {
                while(!ringbuffer.isEmpty)
                    popRingBuf(&ringbuffer);                
            }
        }


        switch (messageHandler.mode)
        {
            case 'A':
            {
                switch(messageHandler.anm_mode)
                {
                    case 0:
                    {
                        shiftPlane_anm(&framebuffer, messageHandler.axis, messageHandler.color, 10*messageHandler.delay);
                        break;
                    }
                }
                break;
            }
            case 'C':
            {
                break;
            }
            case 'D':
            {
                _delay_ms(500);
                break;
            }
            case 'M':
            {
                break;
            }
            /*
            case 'Q':
            {
                break;
            }
            */
            default://Marquee is default
            {
                break;
            }
        }


    }



    //-------EVENT LOOP-----------//
    while(1) 
    {
    
        /*
        for (int i=0; i<4; i++)
        {
            sr_loadData_alt(&ledDriver, data[i], 36);
            delay_ms(500);
        }
        */
    
    /*
    clearFrameBuffer_(&framebuffer);
    frameBufferToBits_DB_NEW(&framebuffer); 
    delay_ms(500);
    setFrameBuffer(&framebuffer);
    frameBufferToBits_DB_NEW(&framebuffer);
    delay_ms(500);
    */
   
/* 
        for (int k=0; k<FBUF_SZ; k++)
        { 
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                
                //clearFrameBuffer_(&framebuffer);
                //setPixel(&framebuffer, i, j, k);
                //setPixel(&framebuffer, 0, 0, 0);
                //frameBufferToBits_DB_NEW(&framebuffer);
                //sr_loadData_alt(&ledDriver, framebuffer.bufferBits[0], 36);
                //_delay_ms(200);
               // timer_delay_ms(400);
                //timer_delay_ms(400);
                
                clearFrameBuffer_(&framebuffer);
                setPixelColor(&framebuffer, i, j, k, RED);
                //setPixel(&framebuffer, 0, 0, 0);
                frameBufferToBits_DB_NEW(&framebuffer);
                //sr_loadData_alt(&ledDriver, framebuffer.bufferBits[0], 36);
                _delay_ms(500);

                clearFrameBuffer_(&framebuffer);
                setPixelColor(&framebuffer, i, j, k, GREEN);
                //setPixel(&framebuffer, 0, 0, 0);
                frameBufferToBits_DB_NEW(&framebuffer);
                //sr_loadData_alt(&ledDriver, framebuffer.bufferBits[0], 36);
                _delay_ms(500);

                clearFrameBuffer_(&framebuffer);
                setPixelColor(&framebuffer, i, j, k, CYAN);
                //setPixel(&framebuffer, 0, 0, 0);
                frameBufferToBits_DB_NEW(&framebuffer);
                //sr_loadData_alt(&ledDriver, framebuffer.bufferBits[0], 36);
                _delay_ms(500);
            }
        }
        }
*/

        
/*
        uint8_t byte = 0xAA;
        uint8_t mask;
        for (int i=7; i>=0; i--)
        {
            PORTD &= ~(1 << PD3);//Triggers on rising edge of clock, must bring low first

            mask = 1 << i;
            if (mask & byte)
                PORTD |= (1 << PD2);
            else
                PORTD &= ~(1 << PD2);

            PORTD |= (1 <<PD3);//Loads data from data pin into shift reg on rising edge
        }
        PORTD &= ~(1 << PD4);
        PORTD |= (1 << PD4);
        */
/*
        clearFrameBuffer_(&framebuffer);
        setPixel(&framebuffer, framebuffer.currLevel, 0, 0);
        frameBufferToBits_DB_NEW(&framebuffer);
        sr_incrementGround(&groundDriver, &framebuffer);
        _delay_ms(3000); 
*/
        //framebuffer.currLevel++;
        //if (framebuffer.currLevel >= 4)
        //    framebuffer.currLevel = 0; 
        

        
        //tritrisGame(&framebuffer, 400);
        //snakeGame(&framebuffer, 1200);
        //sinWave_anm(&framebuffer);
        //clearFrameBuffer_(&framebuffer);


//Begin char display
        char words[64] = "SMAERD EB SMAERD RUOY TEL TNOD  ECALPHCET OT EMOCLEW";//"I DID THE BLIMKY THING ";
                         
//        Character char_A;
//        initCharacter(&char_A, 0, 0, 0, GREEN, getCharacterData('C')/*characters[0]*/);
//        for (int k=0; k<26; k++)
//        for (int k=0; words[k]; k++)
//        {
//        setCharacterData(&char_A, getCharacterData((char)(/*'A'+k*/words[k])));
//       setCharacterColor(&char_A, (uint8_t)rand()%7+1);
//       for (int i=-2; i<FBUF_SZ; i++)
//        {
//            clearFrameBuffer_(&framebuffer);
//            setCharacterPosition(&char_A, i, 0, 0);
//            drawCharacter(&framebuffer, &char_A);
//            frameBufferToBits_DB_NEW(&framebuffer);
//            _delay_ms(250); 
//        }
//        }
//end char display


//*** Commented out on (1/09/16) ****

//        Marquee marquee;
        //initMarquee(&marquee, -1*(26-24)*6, words, RED);
        //initMarquee(&marquee, -1*(strlen(words))*6, words, RED);
//        initMarquee(&marquee, strlen(words)*6-10, words, RED);
//        for (int i=0; i<marquee.text_size; i++)
//        {
//            marquee.text[i].color = (uint8_t)(rand()%7 + 1);
//        }
//        for (int i=0; i<strlen(words)*6*2; i++)
        //for (int i=strlen(words)*6*2-1; i<=0; i--)
//        {
            //for (int j=0; j<marquee.text_size; j++)
            //{
            //    marquee.text[j].color = (uint8_t)(rand()%7 + 1);
            //}
//            clearFrameBuffer_(&framebuffer);
//            drawMarquee(&framebuffer, &marquee);
//            frameBufferToBits_DB_NEW(&framebuffer);
//            _delay_ms(250);
            //marquee.offset++;
//            marquee.offset--;
//        }
      
        char words2[5][64] = {
                                "WELCOME TO TECHPLACE  DONT LET YOUR DREAMS BE DREAMS",
                                "SUCH BLINKY  WOW",
                                "HOLLY WORK ON YOUR BMO",
                                "LED CUBE TWO  ELECTRIC BOOGALOO",
                                "JUST DO IT  FLEXING ACTION"
                            };
        Marquee marquee2;
        
        initMarquee(&marquee2, 0, words2[rand()%5], BLUE);
//        clearFrameBuffer_(&framebuffer);
//        drawMarquee_alt(&framebuffer, &marquee2);
//        frameBufferToBits_DB_NEW(&framebuffer);
//        _delay_ms(500);
        for (int i=0; i<marquee2.text_size; i++)
        {
            if (marquee2.text[i].character_data != NULL)
                marquee2.text[i].color = (uint8_t)(rand()%7 + 1);
        }
        for (marquee2.offset = 0; marquee2.offset < 6* marquee2.text_size+0; marquee2.offset++)
        {
            clearFrameBuffer_(&framebuffer);
            drawMarquee_alt(&framebuffer, &marquee2);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(500);
        }   
        //_delay_ms(20000);

/*
    for (int i=1; i<8; i++)
    {
        clearFrameBuffer_(&framebuffer);
        setFrameBufferColor(&framebuffer, i);
        frameBufferToBits_DB_NEW(&framebuffer);
        _delay_ms(5000);         
    }
*/
/*
        for (int i=FBUF_SZ-2; i>0; i--)
        {
            clearFrameBuffer_(&framebuffer);
            setCharacterPosition(&char_A, i, 0, 0);
            drawCharacter(&framebuffer, &char_A);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(500); 
        }
*/
        //drawCharacter(&framebuffer, &char_A);
        /*
        if (characters[0][0] == 0x00 && characters[0][1] == 0x0C && characters[0][2] == 0x12 &&
            characters[0][3] == 0x1E && characters[0][4] == 0x12 && characters[0][5] == 0x12)
        {
            setFrameBufferColor(&framebuffer, CYAN);
        }
        else
        {
            setFrameBufferColor(&framebuffer, MAGENTA);
        }
        */
        //frameBufferToBits_DB_NEW(&framebuffer);
        //_delay_ms(2000);
/*
        int anm_delay = 150;
        shiftPlane_anm(&framebuffer, 0, rand()%7+1, anm_delay);
        shiftPlane_anm(&framebuffer, 1, rand()%7+1, anm_delay);
        shiftPlane_anm(&framebuffer, 2, rand()%7+1, anm_delay);
*/
/*
        for (int i=1; i<8; i++)
        {
            clearFrameBuffer_(&framebuffer);
            setFrameBufferColor(&framebuffer, (uint8_t)i);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(12000);

            //clearFrameBuffer_(&framebuffer);
            //frameBufferToBits_DB_NEW(&framebuffer);
            //_delay_ms(2000);
        }
*/

/*
        for (int i=0; i<8; i++)
        {
            setFrameBufferColor(&framebuffer, i);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(5000);
        }
*/

        //snake_anm(&framebuffer, 1, 250);
        //ballBounce_anm(&framebuffer, 250);

/*
        Vector3d start, end;
        start.x = 0;
        start.y = 0;
        start.z = 0;
    
        end.x = 5;
        end.y = 5;
        end.z = 5;

        for (int i=0; i<FBUF_SZ; i++)
        {
            clearFrameBuffer_(&framebuffer);
            end.x = i;
            end.y = i;
            end.z = i;
            drawBox(&framebuffer, &start, &end, WHITE);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(100);
        }
        _delay_ms(2000);
        for (int i=FBUF_SZ-1; i>=0; i--)
        {
            clearFrameBuffer_(&framebuffer);
            end.x = i;
            end.y = i;
            end.z = i;
            drawBox(&framebuffer, &start, &end, WHITE);
            frameBufferToBits_DB_NEW(&framebuffer);
            _delay_ms(100);
        }
        _delay_ms(2000);
        //clearFrameBuffer_(&framebuffer);
        //setFrameBufferRandom(&framebuffer);
        //frameBufferToBits_DB_NEW(&framebuffer);
        //_delay_ms(250);
*/


/*
        clearFrameBuffer_(&framebuffer);
        for (int i=0; i <10; i++)
        {
             setPixelColor(&framebuffer, rand()%6, rand()%6, rand()%6, rand()%7 + 1);
        }
        frameBufferToBits_DB_NEW(&framebuffer);
        _delay_ms(250);
*/
/*
        for (int k=0; k<FBUF_SZ; k++)
        { 
            for (int i=0; i<FBUF_SZ; i++)
            {
                for (int j=0; j<FBUF_SZ; j++)
                {
                    clearFrameBuffer_(&framebuffer);
                    setPixel(&framebuffer, i, j, k);
                    frameBufferToBits_DB_NEW(&framebuffer);
                    _delay_ms(1000);
                }
            }
        }
*/


        //clearFrameBuffer_(&framebuffer);
        //setFrameBufferColor(&framebuffer, WHITE);
        //frameBufferToBits_DB_NEW(&framebuffer);        
        //_delay_ms(200);

        //rotatePlane_anm(&framebuffer, 0, 250);
/* 
        clearFrameBuffer_(&framebuffer); 

        setFrameBuffer(&framebuffer);
        frameBufferToBits_DB_NEW(&framebuffer);
        _delay_ms(200);
*/
/*
        clearFrameBuffer_(&framebuffer);
        frameBufferToBits_DB_NEW(&framebuffer);
        _delay_ms(1000);
*/

    }
    return(0);
}
