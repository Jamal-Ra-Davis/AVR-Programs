#include <avr/io.h>
#include "FrameBuffer.h"
#include <stdlib.h>

FrameBuffer* makeFrameBuffer()
{
    FrameBuffer* framebuffer = (FrameBuffer*)malloc(sizeof(FrameBuffer));
    initFrameBuffer(framebuffer);

    return framebuffer;
}
void initFrameBuffer(FrameBuffer* framebuffer)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                framebuffer->buffer[k][i][j] = 0;
            }
        }

        framebuffer->bufferBits[k][0] = 0x00;
        framebuffer->bufferBits[k][1] = 0x00;
    }
    framebuffer->size = FBUF_SZ;
    framebuffer->currLevel = 0;
}


void frameBufferToBits(FrameBuffer* framebuffer)
{
    int k, i, j, offset, i_;
    for (k=0; k<framebuffer->size; k++)
    {
        for (i=0; i<framebuffer->size; i++)
        {
            for (j=0; j<framebuffer->size; j++)
            {
                if (framebuffer->buffer[k][i][j])
                {
                    if (i < 2)
                    {
                        offset = framebuffer->size*i + j;
                        framebuffer->bufferBits[k][i/2] |= (0x80 >> offset);
                    }

                    else
                    {

                        i_ = i-2;
                        offset = framebuffer->size*i_ + j;
                        framebuffer->bufferBits[k][i/2] |= (0x80 >> offset);
                    }
                }
            }
        }
    }    
}

/**********************************/
/*                                */
/*       Drawing Functions        */
/*                                */
/**********************************/

//Clears framebuffer and clears buffer bits
void clearFrameBuffer(FrameBuffer *framebuffer)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                framebuffer->buffer[k][i][j] = 0;
            }
        }
        framebuffer->bufferBits[k][0] = 0x00;
        framebuffer->bufferBits[k][1] = 0x00;
    }
}
//Just clears framebuffer
void clearFrameBuffer_(FrameBuffer *framebuffer)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                framebuffer->buffer[k][i][j] = 0;
            }
        }
    }
}
//Sets pixel in framebuffer
void setPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z)
{
    if ((x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
        return;
    framebuffer->buffer[z][x][y] = 1;
}
//Clears pixel in framebuffer
void clearPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z)
{
    if ((x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
        return;
    framebuffer->buffer[z][x][y] = 0;
}
//Clears pixel in framebuffer
void togglePixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z)
{
    if ((x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
        return;

    if (framebuffer->buffer[z][x][y] == 0)
        framebuffer->buffer[z][x][y] = 1;
    else
        framebuffer->buffer[z][x][y] = 0;
}
