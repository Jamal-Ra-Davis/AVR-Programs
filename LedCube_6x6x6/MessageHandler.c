#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
//#include <string.h>
#include "MessageHandler.h"
//#include "Character.h"
#include "Vector3d.h"
//#include "FrameBuffer.h"


int initMessageHandler(MessageHandler* messageHandler, RingBuffer* ringBuffer)
{
    if (ringBuffer == NULL)
        return -1;

    messageHandler->ringbuffer = ringBuffer;
    messageHandler->mode = 'D';
    messageHandler->anm_mode = 0;

    messageHandler->message_type = 0;
    messageHandler->anm_type = 0;
    messageHandler->anm_idx = 0;
    messageHandler->color = WHITE;
    messageHandler->axis = 0;
    messageHandler->delay = 30;
    messageHandler->loops = 1;
    messageHandler->valid_message = 0;
    return 0;
}
int handleAnimationMessage(MessageHandler* messageHandler, FrameBuffer* framebuffer)
{
    uint8_t anm_type, anm_idx, color, axis, delay, loops;

    if (messageHandler->ringbuffer->isEmpty)
    {
        messageHandler->valid_message = 0;
        return -1;//Incomplete message
    }

    anm_type = popRingBuf(messageHandler->ringbuffer);
    if (anm_type == 'S' || anm_type == 'R')
    {
        if (messageHandler->ringbuffer->isEmpty)
        {
            messageHandler->valid_message = 0;
            return -1;
        }

        anm_idx = popRingBuf(messageHandler->ringbuffer);
        if (anm_idx < 0 || anm_idx > 7)
            return 1;//Invalid message data
        switch(anm_idx)
        {
            case 0:
            {
                if (messageHandler->ringbuffer->cnt < 3)
                {
                    messageHandler->valid_message = 0;
                    return -1;
                }

                color = popRingBuf(messageHandler->ringbuffer);
                axis = popRingBuf(messageHandler->ringbuffer);
                delay = popRingBuf(messageHandler->ringbuffer);
                if (color == 8)
                    color = rand()%7+1;
                if (anm_type == 'S')
                {
                    shiftPlane_anm(framebuffer, axis, color, 10*delay);
                }
                //else
                //{
                //    messageHandler->anm_mode = anm_idx;
                //}
                
                break;
            }
            /*
            case 1:
            {
                break;
            }
            */
            default:
            {
                messageHandler->valid_message = 0;
                return 1;//Invalid message data
            }
        }
    }
    else
    {
        messageHandler->valid_message = 0;
        return 1;//Invalid message data
    }

    if (anm_type == 'R')
    {
        messageHandler->anm_mode = anm_idx;
        messageHandler->mode = 'A';

        messageHandler->anm_type = anm_type;
        messageHandler->anm_idx = anm_idx;
        messageHandler->color = color;
        messageHandler->axis = axis;
        messageHandler->delay = delay;
        messageHandler->loops = loops;
    }
    
    messageHandler->valid_message = 1;
    return 0;
}

int handleDrawingMessage(MessageHandler* messageHandler, FrameBuffer* framebuffer)
{
    if (messageHandler->ringbuffer->isEmpty)
    {
        messageHandler->valid_message = 0;
        return -1;//Incomplete message
    }

    uint8_t drawing_type = popRingBuf(messageHandler->ringbuffer);
    if (drawing_type == 'F')
    {
        //setFrameBufferColor(framebuffer, GREEN);
        //frameBufferToBits_DB_NEW(framebuffer);
        //_delay_ms(500);

        frameBufferToBits_DB_NEW(framebuffer);

        //clearFrameBuffer_(framebuffer);         
        //frameBufferToBits_DB_NEW(framebuffer);
        return 0;
    }
    else if (drawing_type == 'C' || drawing_type == 'S')
    {
        if (messageHandler->ringbuffer->isEmpty)
        {
            messageHandler->valid_message = 0;
            return -1;//Incomplete message
        }
        uint8_t drawing_action = popRingBuf(messageHandler->ringbuffer);
        switch(drawing_action)
        {
            case 'F':
            {
                if (drawing_type == 'C')
                {
                    clearFrameBuffer_(framebuffer);
                    return 0;
                }
                else
                {
                    if (messageHandler->ringbuffer->isEmpty)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t color = popRingBuf(messageHandler->ringbuffer);
                    if (color > 8)
                    {
                        messageHandler->valid_message = 0;
                        return 1;
                    }
                    if (color == 8)
                    {
                        color = rand()%7 + 1;
                    }
                    setFrameBufferColor(framebuffer, color);
                    return 0;
                }
                break;
            }
            case 'V':
            {
                if (drawing_type == 'C')
                {
                    if (messageHandler->ringbuffer->cnt < 3)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t xPos = popRingBuf(messageHandler->ringbuffer);
                    uint8_t yPos = popRingBuf(messageHandler->ringbuffer);
                    uint8_t zPos = popRingBuf(messageHandler->ringbuffer);
                    
                    //Check x, y, z values 

                    clearPixel(framebuffer, xPos, yPos, zPos);
                }
                else
                {
                    if (messageHandler->ringbuffer->cnt < 4)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t color = popRingBuf(messageHandler->ringbuffer);
                    uint8_t xPos = popRingBuf(messageHandler->ringbuffer);
                    uint8_t yPos = popRingBuf(messageHandler->ringbuffer);
                    uint8_t zPos = popRingBuf(messageHandler->ringbuffer);

                    if (color > 8)
                    {
                        messageHandler->valid_message = 0;
                        return 1;
                    }
                    if (color == 8)
                    {
                        color = rand()%7 + 1;
                    }

                    //Check x, y, z values 
                    //setFrameBufferColor(framebuffer, CYAN);
                    //frameBufferToBits_DB_NEW(framebuffer);
                    //_delay_ms(500);
                    //clearFrameBuffer_(framebuffer);
                    //frameBufferToBits_DB_NEW(framebuffer); 

                    setPixelColor(framebuffer, xPos, yPos, zPos, color);
                    return 0;
                }
                break;
            }
            case 'L':
            {
                if (drawing_type == 'C')
                {
                    if (messageHandler->ringbuffer->cnt < 3)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t axis = popRingBuf(messageHandler->ringbuffer);
                    uint8_t pos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t pos2 = popRingBuf(messageHandler->ringbuffer);

                    //Check axis and pos values 

                    drawLine(framebuffer, axis, pos1, pos2, 0);
                }
                else
                {
                    if (messageHandler->ringbuffer->cnt < 4)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t color = popRingBuf(messageHandler->ringbuffer);
                    uint8_t axis = popRingBuf(messageHandler->ringbuffer);
                    uint8_t pos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t pos2 = popRingBuf(messageHandler->ringbuffer);

                    if (color > 8)
                    {
                        messageHandler->valid_message = 0;
                        return 1;
                    }
                    if (color == 8)
                    {
                        color = rand()%7 + 1;
                    }

                    //Check x, y, z values 
                    drawLine(framebuffer, axis, pos1, pos2, color);
                    return 0;
                }
                break;
            }
            case 'P':
            {
                if (drawing_type == 'C')
                {
                    if (messageHandler->ringbuffer->cnt < 2)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t axis = popRingBuf(messageHandler->ringbuffer);
                    uint8_t pos = popRingBuf(messageHandler->ringbuffer);

                    //Check axis and pos values 
                    drawPlane(framebuffer, axis, pos, 0);
                }
                else
                {
                    if (messageHandler->ringbuffer->cnt < 3)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t color = popRingBuf(messageHandler->ringbuffer);
                    uint8_t axis = popRingBuf(messageHandler->ringbuffer);
                    uint8_t pos = popRingBuf(messageHandler->ringbuffer);

                    if (color > 8)
                    {
                        messageHandler->valid_message = 0;
                        return 1;
                    }
                    if (color == 8)
                    {
                        color = rand()%7 + 1;
                    }

                    //Check x, y, z values 
                    drawPlane(framebuffer, axis, pos, color);
                    return 0;
                }
                break;
            }
            case 'B':
            {
                if (drawing_type == 'C')
                {
                    if (messageHandler->ringbuffer->cnt < 6)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t xPos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t yPos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t zPos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t xPos2 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t yPos2 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t zPos2 = popRingBuf(messageHandler->ringbuffer);

                    //Check axis and pos values 
                    Vector3d start, end;
                    setVector3d(&start, xPos1, yPos1, zPos1);
                    setVector3d(&end, xPos2, yPos2, zPos2);
                    drawBox(framebuffer, &start, &end, 0);
                }
                else
                {
                    if (messageHandler->ringbuffer->cnt < 7)
                    {
                        messageHandler->valid_message = 0;
                        return -1;
                    }
                    uint8_t color = popRingBuf(messageHandler->ringbuffer);
                    uint8_t xPos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t yPos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t zPos1 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t xPos2 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t yPos2 = popRingBuf(messageHandler->ringbuffer);
                    uint8_t zPos2 = popRingBuf(messageHandler->ringbuffer);

                    if (color > 8)
                    {
                        messageHandler->valid_message = 0;
                        return 1;
                    }
                    if (color == 8)
                    {
                        color = rand()%7 + 1;
                    }

                    //Check x, y, z values 
                    Vector3d start, end;
                    setVector3d(&start, xPos1, yPos1, zPos1);
                    setVector3d(&end, xPos2, yPos2, zPos2);
                    drawBox(framebuffer, &start, &end, 0);
                    return 0;
                }
                break;
            }
            default:
            {
                messageHandler->valid_message = 0;
                return 1;
                break;
            }
        }
    }
    else
    {
        messageHandler->valid_message = 0;
        return 1;    
    }   
 
    return 0;
}


