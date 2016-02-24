#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "FrameBuffer.h"
#include "delay.h"
#include "Snake.h"
#include "ButtonController.h"
#include "Vector3d.h"
#include "Tritris.h"

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

        //framebuffer->bufferBits[k][0] = 0x00;
        //framebuffer->bufferBits[k][1] = 0x00;
        for (int c=0; c<3; c++)
        {
            for (int regs=0; regs<FBUF_REGS; regs++)
            {
                framebuffer->bufferBits[k][c][regs] = 0x00;
            }
        }
    }
    framebuffer->size = FBUF_SZ;
    framebuffer->currLevel = 0;
}


void frameBufferToBits(FrameBuffer* framebuffer)
{
/*
    int k, i, j, offset, i_;
    for (k=0; k<framebuffer->size; k++)
    {
        framebuffer->bufferBits[k][0] = 0x00;
        framebuffer->bufferBits[k][1] = 0x00;
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
*/
}


void frameBufferToBits_DB(FrameBuffer* framebuffer)
{
/*
    //Will actually need to initialze temp buffer bits to 0
    uint8_t temp[FBUF_SZ][FBUF_REGS];// = {
                            //{0x00, 0x00},
                            //{0x00, 0x00},
                            //{0x00, 0x00},
                            //{0x00, 0x00}
                        //};
                        
    int k, i, j, offset, i_;
    for (k=0; k<framebuffer->size; k++)
    {
        //Could initilaze temp buffer here
        //
        //    Initilzation code...
        //
        for (i=0; i<framebuffer->size; i++)
        {
            for (j=0; j<framebuffer->size; j++)
            {
                if (framebuffer->buffer[k][i][j])
                {
                    if (i < 2)
                    {
                        offset = framebuffer->size*i + j;
                        temp[k][i/2] |= (0x80 >> offset);
                    }

                    else
                    {

                        i_ = i-2;
                        offset = framebuffer->size*i_ + j;
                        temp[k][i/2] |= (0x80 >> offset);
                    }
                }
            }
        }
    }
    for (int i=0; i<FBUF_SZ; i++)
    {
        framebuffer->bufferBits[i][0] = temp[i][0];
        framebuffer->bufferBits[i][1] = temp[i][1];
    }
*/
}

void frameBufferToBits_DB_NEW(FrameBuffer* framebuffer)
{
    //Will actually need to initialze temp buffer bits to 0
    uint8_t temp[FBUF_SZ][3][FBUF_REGS];
    int k, i, j, offset, reg_idx;

    //reg_idx = 0;
    //offset = FBUF_SZ*FBUF_SZ - 1;
    //if (offset == -1)
    //    offset = 7;

    for (k=0; k<framebuffer->size; k++)
    {
        reg_idx = 0;
        offset = (FBUF_SZ*FBUF_SZ%8) - 1;
        if (offset == -1)
            offset = 7;


        //Iniitalize temp buffer
        for (int c=0; c<3; c++)
        {
            for (int regs=0; regs<FBUF_REGS; regs++)
            {
                temp[k][c][regs] = 0x00;
            }
        }

        for (i=0; i<framebuffer->size; i++)
        {
            for (j=0; j<framebuffer->size; j++)
            {
                //for (int c=0; c<3; c++)
                //{
                    /*
                    if (framebuffer->buffer[k][i][j][c])
                        temp[k][c][reg_idx] |= (1 << offset);
                    */
                    colorToBits(temp[k], reg_idx, offset, framebuffer->buffer[k][i][j]);
                //}

                offset--;
                if (offset < 0)
                {
                    offset = 7;
                    reg_idx++;
                }
            }
        }
    }
    for (int i=0; i<FBUF_SZ; i++)
    {
        for (int c=0; c<3; c++)
        {
            for (int regs=0; regs<FBUF_REGS; regs++)
            {
                framebuffer->bufferBits[i][c][regs] = temp[i][c][regs];
            }
        }
    }
}
void colorToBits(uint8_t color_array[3][FBUF_REGS], int reg_idx, int offset, uint8_t color)
{
    for (int i=2; i>=0; i--)
    {
        int color_idx = 2 - i;
        //int color_idx = i;
        if (color & (1 << i))
            color_array[color_idx][reg_idx] |= (1 << offset);
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
        //framebuffer->bufferBits[k][0] = 0x00;
        //framebuffer->bufferBits[k][1] = 0x00;
        for (int c=0; c<3; c++)
        {        
            for (int regs=0; regs<FBUF_REGS; regs++)
            {
                framebuffer->bufferBits[k][c][regs] = 0x00;
            }
        }
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
//Sets entire framebuffer
void setFrameBuffer(FrameBuffer *framebuffer)
{
    for (int k=0; k<framebuffer->size; k++)
    {
        for (int i=0; i<framebuffer->size; i++)
        {
            for (int j=0; j<framebuffer->size; j++)
            {
                framebuffer->buffer[k][i][j] = 1;
            }
        }
    }
}
void setFrameBufferColor(FrameBuffer *framebuffer, uint8_t color)
{
    for (int k=0; k<framebuffer->size; k++)
    {
        for (int i=0; i<framebuffer->size; i++)
        {
            for (int j=0; j<framebuffer->size; j++)
            {
                framebuffer->buffer[k][i][j] = color;
            }
        }
    }
}
void setFrameBufferRandom(FrameBuffer *framebuffer)
{
    for (int k=0; k<framebuffer->size; k++)
    {
        for (int i=0; i<framebuffer->size; i++)
        {
            for (int j=0; j<framebuffer->size; j++)
            {
                framebuffer->buffer[k][i][j] = rand()%7 + 1;
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
void setPixelColor(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z, uint8_t color)
{
    if ((x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
        return;
    //if (color >= COLOR_RNG)
    //return;
    framebuffer->buffer[z][x][y] = color;
}
//Clears pixel in framebuffer
void clearPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z)
{
    if ((x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
        return;
    framebuffer->buffer[z][x][y] = 0;
}
//Toggles pixel in framebuffer
void togglePixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z)
{
    if ((x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
        return;

    if (framebuffer->buffer[z][x][y] == 0)
        framebuffer->buffer[z][x][y] = 1;
    else
        framebuffer->buffer[z][x][y] = 0;
}

void drawVector3d(FrameBuffer *framebuffer, Vector3d point)
{
    setPixel(framebuffer, (uint8_t)point.x, (uint8_t)point.y, (uint8_t)point.z);
}
void drawLine(FrameBuffer *framebuffer, uint8_t axis, uint8_t x, uint8_t y)
{
    //assume Z-axis for now
    switch (axis)
    {
        //Normal to XZ-plane
        case 0:
        {
            for (int k=0; k<framebuffer->size; k++)
                setPixel(framebuffer, x, k, y);
            break;
        }
        //Normal to YZ-plane
        case 1:
        {
            for (int k=0; k<framebuffer->size; k++)
                setPixel(framebuffer, k, x, y);
            break;
        }
        //Normal to XY-plane
        default:
        {
            for (int k=0; k<framebuffer->size; k++)
                setPixel(framebuffer, x, y, k);
            break;
        }
    }
}
void drawPlane(FrameBuffer *framebuffer, uint8_t axis, uint8_t level)
{
    switch (axis)
    {
        case 0:
        {
            for (int i=0; i<framebuffer->size; i++)
            {
                for (int j=0; j<framebuffer->size; j++)
                {
                    setPixel(framebuffer, i, level, j);
                }
            }
            break;
        }
        case 1:
        {
            for (int i=0; i<framebuffer->size; i++)
            {
                for (int j=0; j<framebuffer->size; j++)
                {
                    setPixel(framebuffer, level, i, j);
                }
            }
            break;
        }
        default:
        {
            for (int i=0; i<framebuffer->size; i++)
            {
                for (int j=0; j<framebuffer->size; j++)
                {
                    setPixel(framebuffer, i, j, level);
                }
            }
            break;
        }
    }
}
void clearPlane(FrameBuffer *framebuffer, uint8_t axis, uint8_t level)
{
    switch (axis)
    {
        case 0://XZ
        {
            for (int i=0; i<framebuffer->size; i++)
            {
                for (int j=0; j<framebuffer->size; j++)
                {
                    clearPixel(framebuffer, i, level, j);
                }
            }
            break;
        }
        case 1://YZ
        {
            for (int i=0; i<framebuffer->size; i++)
            {
                for (int j=0; j<framebuffer->size; j++)
                {
                    clearPixel(framebuffer, level, i, j);
                }
            }
            break;
        }
        default://XY Plane
        {
            for (int i=0; i<framebuffer->size; i++)
            {
                for (int j=0; j<framebuffer->size; j++)
                {
                    clearPixel(framebuffer, i, j, level);
                }
            }
            break;
        }
    }
}

void drawBox(FrameBuffer *framebuffer, Vector3d* start, Vector3d* end, uint8_t color)
{
    int x_start, y_start, z_start;
    int x_end, y_end, z_end;

    if (start->x > end->x)
    {
        x_start = end->x;
        x_end = start->x;
    }
    else
    {
        x_start = start->x;
        x_end = end->x;
    }
    
    if (start->y > end->y)
    {
        y_start = end->y;
        y_end = start->y;
    }
    else
    {
        y_start = start->y;
        y_end = end->y;
    }
    
    if (start->z > end->z)
    {
        z_start = end->z;
        z_end = start->z;
    }
    else
    {
        z_start = start->z;
        z_end = end->z;
    }


    for (int i=x_start; i <= x_end; i++)
    {
        setPixelColor(framebuffer, i, y_start, z_start, color);
        setPixelColor(framebuffer, i, y_end, z_start, color);
        setPixelColor(framebuffer, i, y_start, z_end, color);
        setPixelColor(framebuffer, i, y_end, z_end, color);
    }
    for (int i=y_start; i <= y_end; i++)
    {
        setPixelColor(framebuffer, x_start, i, z_start, color);
        setPixelColor(framebuffer, x_end, i, z_start, color);
        setPixelColor(framebuffer, x_start, i, z_end, color);
        setPixelColor(framebuffer, x_end, i, z_end, color);
    }
    for (int i=z_start; i <= z_end; i++)
    {
        setPixelColor(framebuffer, x_start, y_start, i, color);
        setPixelColor(framebuffer, x_start, y_end, i, color);
        setPixelColor(framebuffer, x_end, y_start, i, color);
        setPixelColor(framebuffer, x_end, y_end, i, color);
    }
}



//Need to change function definition for function to be scaleable, 16 bit number input assumes 4x4x4 cube
void drawBinaryNumber(FrameBuffer *framebuffer, uint16_t number, uint8_t axis, uint8_t level)
{
    for (int i=0; i<16; i++)
    {
        //uint16_t mask = (1 << 15-i);
        uint8_t row = i/4;
        uint8_t col = i - 4*row;


        switch (axis)
        {
            case 0:
            {
                if (number & (1 << (15-i)))
                    setPixel(framebuffer, col, level, 3-row);
                else
                    clearPixel(framebuffer, col, level, 3-row);
                break;
            }
            case 1:
            {
                if (number & (1 << (15-i)))
                    setPixel(framebuffer, level, col, 3-row);
                else
                    clearPixel(framebuffer, level, col, 3-row);
                break;
            }
            default:
            {
                if (number & (1 << (15-i)))
                    setPixel(framebuffer, row, col, level);
                else
                    clearPixel(framebuffer, row, col, level);
                break;
            }
        }
    }

/*
    int sel;
    for (int x=0; x<4; x++)
    {
        for (int y=0; y<4; y++)
        {
            int id = x*4 + y;
            int id_;
            if (id >= 8)
            {
                id_ = id - 8;
                sel = 1;
            }
            else
            {
                id_ = id;
                sel = 0;
            }
            
            //if (number[sel] & (0x80 >> id_))
            if (x==2)
                setPixel(framebuffer, x, y, 0);
            else   
                clearPixel(framebuffer, x, y, 0);
        }
    }
*/
}

void shiftPlane_anm(FrameBuffer *framebuffer, uint8_t axis, uint8_t color, int delay)
{
    switch(axis)
    {
        //Normal to XZ-plane 
        case 0:
        {
            for (int k=0; k<framebuffer->size; k++)
            {
                clearFrameBuffer_(framebuffer);
                for (int i=0; i<framebuffer->size; i++)
                {
                    for (int j=0; j<framebuffer->size; j++)
                    {
                        setPixelColor(framebuffer, i, k, j, color);
                    }
                }
                frameBufferToBits_DB_NEW(framebuffer);
                delay_ms(delay);
            }
            for (int k=framebuffer->size-2; k>=1; k--)
            {
                clearFrameBuffer_(framebuffer);
                for (int i=0; i<framebuffer->size; i++)
                {
                    for (int j=0; j<framebuffer->size; j++)
                    {
                        setPixelColor(framebuffer, i, k, j, color);
                    }
                }
                frameBufferToBits_DB_NEW(framebuffer);
                delay_ms(delay);
            }
            break;
        }
        //Normal to YZ-plane
        case 1:
        {
            for (int k=0; k<framebuffer->size; k++)
            {
                clearFrameBuffer_(framebuffer);
                for (int i=0; i<framebuffer->size; i++)
                {
                    for (int j=0; j<framebuffer->size; j++)
                    {
                        setPixelColor(framebuffer, k, i, j, color);
                    }
                }
                frameBufferToBits_DB_NEW(framebuffer);
                delay_ms(delay);
            }
            for (int k=framebuffer->size-2; k>=1; k--)
            {
                clearFrameBuffer_(framebuffer);
                for (int i=0; i<framebuffer->size; i++)
                {
                    for (int j=0; j<framebuffer->size; j++)
                    {
                        setPixelColor(framebuffer, k, i, j, color);
                    }
                }
                frameBufferToBits_DB_NEW(framebuffer);
                delay_ms(delay);
            }
            break;
        }
        //Normal to XY-plane
        default:
        {
            for (int k=0; k<framebuffer->size; k++)
            {
                clearFrameBuffer_(framebuffer);
                for (int i=0; i<framebuffer->size; i++)
                {
                    for (int j=0; j<framebuffer->size; j++)
                    {
                        setPixelColor(framebuffer, i, j, k, color);
                    }
                }
                frameBufferToBits_DB_NEW(framebuffer);
                delay_ms(delay);
            }
            for (int k=framebuffer->size-2; k>=1; k--)
            {
                clearFrameBuffer_(framebuffer);
                for (int i=0; i<framebuffer->size; i++)
                {
                    for (int j=0; j<framebuffer->size; j++)
                    {
                        setPixelColor(framebuffer, i, j, k, color);
                    }
                }
                frameBufferToBits_DB_NEW(framebuffer);
                delay_ms(delay);
            }
            break;
        }
    }
}
//Function hardcoded for 4x4x4 led cube
void rotatePlane_anm(FrameBuffer *framebuffer, uint8_t axis, uint8_t color, int delay)
{
    clearFrameBuffer_(framebuffer);
    drawLine(framebuffer, axis, 0, 3);
    drawLine(framebuffer, axis, 1, 2);
    drawLine(framebuffer, axis, 2, 1);
    drawLine(framebuffer, axis, 3, 0);
    frameBufferToBits_DB_NEW(framebuffer);
    delay_ms(delay);

    clearFrameBuffer_(framebuffer);
    drawLine(framebuffer, axis, 1, 3);
    drawLine(framebuffer, axis, 1, 2);
    drawLine(framebuffer, axis, 2, 1);
    drawLine(framebuffer, axis, 2, 0);
    frameBufferToBits_DB_NEW(framebuffer);
    delay_ms(delay);   

    clearFrameBuffer_(framebuffer);
    drawLine(framebuffer, axis, 2, 3);
    drawLine(framebuffer, axis, 2, 2);
    drawLine(framebuffer, axis, 1, 1);
    drawLine(framebuffer, axis, 1, 0);
    frameBufferToBits_DB_NEW(framebuffer);
    delay_ms(delay);

    clearFrameBuffer_(framebuffer);
    drawLine(framebuffer, axis, 0, 0);
    drawLine(framebuffer, axis, 1, 1);
    drawLine(framebuffer, axis, 2, 2);
    drawLine(framebuffer, axis, 3, 3);
    frameBufferToBits_DB_NEW(framebuffer);
    delay_ms(delay);

    clearFrameBuffer_(framebuffer);
    drawLine(framebuffer, axis, 0, 1);
    drawLine(framebuffer, axis, 1, 1);
    drawLine(framebuffer, axis, 2, 2);
    drawLine(framebuffer, axis, 3, 2);
    frameBufferToBits_DB_NEW(framebuffer);
    delay_ms(delay);

    clearFrameBuffer_(framebuffer);
    drawLine(framebuffer, axis, 0, 2);
    drawLine(framebuffer, axis, 1, 2);
    drawLine(framebuffer, axis, 2, 1);
    drawLine(framebuffer, axis, 3, 1);
    frameBufferToBits_DB_NEW(framebuffer);
    delay_ms(delay);
}
//Function hardcoded for 4x4x4 led cube
void snake_anm(FrameBuffer *framebuffer, uint8_t loops, int delay)
{
    delay /= 2; 
    Snake snake;    
    initSnake(&snake, 0, 0, 0);
    for (int l=0; l<loops; l++)
    {
        //int x = framebuffer->size-1;
        int x = 0; 
        int y = 0; 
        int z = 0;
        //up
        for (; z < framebuffer->size; z++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            //setPixel(framebuffer, x, y, z);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
            
        }
        z--; 
        //right
        for (y=1; y < framebuffer->size; y++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            //setPixel(framebuffer, x, y, z);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        y--;
        z--; 
        //down
        for (; z >= 0; z--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            //setPixel(framebuffer, x, y, z);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        z++;
        y--;
        //left
        for (; y >= 1; y--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        y++;
        z++;
        //up
        for (; z < framebuffer->size-1; z++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);

        }
        z--;
        y++;
        //right
        for (; y < framebuffer->size-1; y++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        y--;
        z--;
        //down
        for (; z >= 1; z--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        z++;
        //forward
        x++;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //left
        y--;  
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //up
        z++;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //right
        y++;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //forward
        x++;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //left
        y--;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //down
        z--;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //right
        y++;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //forward
        x++;
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        //up
        for (; z < framebuffer->size-1; z++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);

        }
        z--;
        y--;
        //left
        for (; y >= 1; y--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);

        }
        y++;
        z--;
        //down
        for (; z >= 0; z--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        z++; 
        y++;
        //right
        for (; y < framebuffer->size; y++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        y--;
        z++;
        //up
        for (; z < framebuffer->size; z++)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        z--;
        y--;
        //left
        for (; y >= 0; y--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
        y++;
        z--;
        //left
        for (; z >= 0; z--)
        {
            clearFrameBuffer_(framebuffer);
            setHead(&snake, x, y, z);
            drawSnake(framebuffer, &snake);
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }
 
    }
    
}
void tallyScore_anm(FrameBuffer *framebuffer, int score, uint8_t axis, int delay)
{
    int loops = score/16 + 1;
    int tallies = 0;

    clearFrameBuffer_(framebuffer);
    //for (int cnt=0; cnt<loops; cnt++)
    while(score)
    {
        if (tallies)
        {
            int col = (tallies-1)/FBUF_SZ;
            int row = tallies - 1 - col*FBUF_SZ;

            switch (axis)
            {
                case 0:
                    setPixel(framebuffer, col, 0, row);
                    break;
                case 1:
                    setPixel(framebuffer, 0, col, row);
                    break;  
                default:
                    setPixel(framebuffer, FBUF_SZ-1-row, col, 0);
                    break;
            }
            //setPixel(framebuffer, 0, col, row);
            frameBufferToBits_DB(framebuffer);
        }
        //clearPlane(framebuffer, 1, FBUF_SZ-1);
        clearPlane(framebuffer, axis, FBUF_SZ-1);
        for (int i=0; i<FBUF_SZ&&score; i++)
        {
            for (int j=0; j<FBUF_SZ&&score; j++)
            {
                score--;
                
                switch (axis)
                {
                    case 0:
                        setPixel(framebuffer, i, FBUF_SZ-1, j);
                        break;
                    case 1:
                        setPixel(framebuffer, FBUF_SZ-1, i, j);
                        break;
                    default:
                        setPixel(framebuffer, FBUF_SZ-1-j, i, FBUF_SZ-1);
                        break;
                }
                
                //setPixel(framebuffer, FBUF_SZ-1, i, j);
                frameBufferToBits_DB(framebuffer);
                delay_ms(delay);
            }
        }
        //int col = tallies/FBUF_SZ;
        //int row = tallies - col*FBUF_SZ;
        tallies++;
        //clearPlane(framebuffer, 1, FBUF_SZ-1);
        //setPixel(framebuffer, 0, col, row);
        //frameBufferToBits_DB(framebuffer);
        //delay_ms(delay);   
    }
    delay_ms(delay*15);
}
void sinWave_anm(FrameBuffer *framebuffer)
{
    float inc = M_PI/8.0;
    //for (int t=0; t<20; t++)
    //{
        clearFrameBuffer_(framebuffer);
        for (int x=0; x<FBUF_SZ; x++)
        {
            float value = /*(FBUF_SZ/2)*sin(x*inc) + */FBUF_SZ/2;
            setPixel(framebuffer, x, 0, (int)value);
        }
        frameBufferToBits_DB(framebuffer);
        delay_ms(500);
    //}
}
void ballBounce_anm(FrameBuffer *framebuffer, int delay)
{

    int x, y, z;
    int x_vel, y_vel, z_vel;
    uint8_t color = rand()%7 + 1;
    x = rand()%6;
    y = rand()%6;
    z = rand()%6;

    if (rand()%2)
        x_vel = 1;
    else     
        x_vel = -1;

    if (rand()%2)
        y_vel = 1;
    else
        y_vel = -1;

    if (rand()%2)
        z_vel = 1;
    else
        z_vel = -1;    

    while (1)
    {
        uint8_t color_change = 0;

        x += x_vel;
        if (x < 0)
        {
            x_vel *= -1;
            color_change = 1;
            x = 0;
        }
        if (x >= FBUF_SZ)
        {
            x_vel *= -1;
            color_change = 1;
            x = FBUF_SZ - 1;
        }

        y += y_vel;
        if (y < 0)
        {
            y_vel *= -1;
            color_change = 1;
            y = 0;
        }
        if (y >= FBUF_SZ)
        {
            y_vel *= -1;
            color_change = 1;
            y = FBUF_SZ - 1;
        }

        z += z_vel;
        if (z < 0)
        {
            z_vel *= -1;
            color_change = 1;
            z = 0;
        }
        if (z >= FBUF_SZ)
        {
            z_vel *= -1;
            color_change = 1;
            z = FBUF_SZ - 1;
        }

        if (color_change)
            color = rand()%7 + 1;


        clearFrameBuffer_(framebuffer);
        setPixelColor(framebuffer, (uint8_t)x, (uint8_t)y, (uint8_t)z, color);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(delay);
    }
}
void boxExpand_anm(FrameBuffer *framebuffer, uint8_t color, int delay)
{
    
}


void controlPixel(FrameBuffer *framebuffer, int delay)
{
    ButtonController controller;
    initButtonController(&controller);
    int x = 0, y = 0, z = 0;
    Snake snake;
    initSnake(&snake, x, y, z);
    while (1)
    {
        /*
        if (buttonPressed(&controller, 'F'))
            x--;
        else if (buttonPressed(&controller, 'B'))
            x++;
        if (buttonPressed(&controller, 'L'))
            y--;
        else if (buttonPressed(&controller, 'R'))
            y++;
        if (buttonPressed(&controller, 'U'))
            z++;
        else if (buttonPressed(&controller, 'D'))
            z--;
        */
        if (buttonPressedDebounce(&controller, 'F'))
            x--;
        else if (buttonPressedDebounce(&controller, 'B'))
            x++;
        if (buttonPressedDebounce(&controller, 'L'))
            y--;
        else if (buttonPressedDebounce(&controller, 'R'))
            y++;
        if (buttonPressedDebounce(&controller, 'U'))
            z++;
        else if (buttonPressedDebounce(&controller, 'D'))
            z--;

        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        if (z < 0)
            z = 0;

        if (x >= FBUF_SZ)
            x = FBUF_SZ-1;
        if (y >= FBUF_SZ)
            y = FBUF_SZ-1;
        if (z >= FBUF_SZ)
            z = FBUF_SZ-1; 
        
        //clearFrameBuffer_(framebuffer);
        //setPixel(framebuffer, x, y, z);
        //frameBufferToBits_DB(framebuffer);    

        clearFrameBuffer_(framebuffer);
        setHead(&snake, x, y, z);
        drawSnake(framebuffer, &snake);
        frameBufferToBits_DB(framebuffer);
        delay_ms(delay);      
    }
}
void snakeGame(FrameBuffer *framebuffer, int delay)
{
    ButtonController controller;
    initButtonController(&controller);

    int score = 0;
    Vector3d dot;
    //int deadPxNum = 0;
    //Vector3d deadPixels[deadPxNum];
    //setVector3d(deadPixels+0, 3, 0, 0);
    //setVector3d(deadPixels+1, 2, 3, 2);

    int x = 1, y = 1, z = 1;
    Snake snake;
    initSnake(&snake, x, y, z);
    //snakeInitTest(framebuffer, &snake);

    int loop = 1;
    while (loop)
    {
        loop = 0;
        setVector3d(&dot, rand()%framebuffer->size, rand()%framebuffer->size, rand()%framebuffer->size);
        if (pointOverlapsSnake(&snake, dot))
            loop = 1;
        /*
        for (int i=0; i<deadPxNum; i++)
        {
            if ((dot.x == deadPixels[i].x)&&(dot.y == deadPixels[i].y)&&(dot.z == deadPixels[i].z))
                loop = 1;
        }
        */
    }
    uint8_t dot_color = RED;//(rand())%6+1;
    int dir = 2;/*
                    0 = forward;
                    1 = left;
                    2 = back;
                    3 = right;
                    4 = up;
                    5 = down;
                */
    int start = 1;
    while (1)
    {
        uint8_t b_st = getButtonStates(&controller);
        if (b_st)
        {
            if (b_st & (1 << controller.f_id))
                dir = 0;
            if (b_st & (1 << controller.b_id))
                dir = 2;
            if (b_st & (1 << controller.l_id))
                dir = 1;
            if (b_st & (1 << controller.r_id))
                dir = 3;
            if (b_st & (1 << controller.u_id))
                dir = 4;
            if (b_st & (1 << controller.d_id))
                dir = 5;
            break;
        }

        clearFrameBuffer_(framebuffer);
        setPixelColor(framebuffer, x, y, z, WHITE);
        frameBufferToBits_DB_NEW(framebuffer);    
    }
    while(1)
    {
        uint8_t states = getButtonStates(&controller);
        int step = 2;
        for (int j=0; j<delay/step; j++)
        {
            delay_ms(step);
            states |= getButtonStates(&controller);
        }

        if ((dir == 0)||(dir == 2))//Moving forward or backwards
        {
            if (states & (1 << controller.l_id))
                dir = 1;
            if (states & (1 << controller.r_id))
                dir = 3;
            if (states & (1 << controller.u_id))
                dir = 4;
            if (states & (1 << controller.d_id))
                dir = 5;
            
        }
        else if ((dir == 1)||(dir == 3))//Moving left or right
        {
            if (states & (1 << controller.f_id))
                dir = 0;
            if (states & (1 << controller.b_id))
                dir = 2;
            if (states & (1 << controller.u_id))
                dir = 4;
            if (states & (1 << controller.d_id))
                dir = 5;
        }
        else if ((dir == 4)||(dir == 5))//Moving up or down
        {
            if (states & (1 << controller.l_id))
                dir = 1;
            if (states & (1 << controller.r_id))
                dir = 3;
            if (states & (1 << controller.f_id))
                dir = 0;
            if (states & (1 << controller.b_id))
                dir = 2;
        }

        switch (dir)
        {
            case 0://Forward
            {
                x--;
                break;
            }
            case 1://Left
            {
                y--;
                break;
            }
            case 2://Back
            {
                x++;
                break;
            }
            case 3://Right
            {
                y++;
                break;
            }
            case 4://Up
            {
                z++;
                break;
            }
            case 5://Down
            {
                z--;
                break;
            }
        } 
        
        //Check if bounds have been exceeded 
        if ((x < 0)||(y < 0)||(z < 0)||(x >= framebuffer->size)||(y >= framebuffer->size)||(z >= framebuffer->size))
            break;
        if (!start)
        {
            if (snakeCrossesBody(&snake))
                break;
        }
      
        //Once new position has been decided, actually push updates to snake 
        setHead(&snake, x, y, z);
 
        //Snake ate dot
        if ((x == dot.x)&&(y == dot.y)&&(z == dot.z))
        {
            //Increase score
            score++;    

            //increment snake
            incrementSnake(&snake, dot_color);
 
            //find new location for dot
            loop = 1;
            while (loop)
            {
                loop = 0;
                setVector3d(&dot, rand()%framebuffer->size, rand()%framebuffer->size, rand()%framebuffer->size);
                if (pointOverlapsSnake(&snake, dot))
                    loop = 1;
                /*
                for (int i=0; i<deadPxNum; i++)
                {
                    if ((dot.x == deadPixels[i].x)&&(dot.y == deadPixels[i].y)&&(dot.z == deadPixels[i].z))
                    loop = 1;
                }
                */
            }
            dot_color = (rand()%6)+1;
        }
     
        start = 0;


        clearFrameBuffer_(framebuffer);

        setPixelColor(framebuffer, dot.x, dot.y, dot.z, dot_color);         //Draw dot
        drawSnake(framebuffer, &snake);         //Draw snake

        frameBufferToBits_DB_NEW(framebuffer);
        //delay_ms(delay);
    }
    
    //Flash cube to signify game over
    for (int i=0; i<5; i++)
    {
        setFrameBuffer(framebuffer);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(8*25);
    
        clearFrameBuffer_(framebuffer);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(8*25);
    }
    
    //Tally points
    tallyScore_anm(framebuffer, score, 1, 50);

    //delay_ms(1000);
}
void tritrisGame(FrameBuffer *framebuffer, int delay)
{
    ButtonController controller;
    initButtonController(&controller);

    //uint8_t game_space[4][4][4];
    uint8_t new_block = 1;

    TritrisArea area;
    initTritrisArea(&area);

    /*
    for (int i=0; i<FBUF_SZ; i++)
    {
        for (int j=0; j<FBUF_SZ; j++)
        {
            area.gameArea[0][i][j] = (rand()%7)+1;
        }
    }
    area.gameArea[0][2][2] = 0;
    */

    Tritris corner;
    initTritrisBlock(&corner, 1, (rand()%7)+1);
    Vector3d pos;
    setVector3d(&pos, 1, 1, FBUF_SZ);
    setBlockPosition(&corner, pos);
    uint8_t old_states = 0x00;

    uint8_t delay_cnt = 0;

    int score = 0;

/*
*/
    while(1)
    {
        if (new_block)
        {
            initTritrisBlock(&corner, rand()%2, (rand()%7)+1);
            setVector3d(&pos, 1, 1, FBUF_SZ);
            setBlockPosition(&corner, pos);

            new_block = 0; 
        }

        uint8_t states = getButtonStates(&controller);
        int step = 2;
        for (int j=0; j<delay/step; j++)
        {
            delay_ms(step);
            states |= getButtonStates(&controller);
        }
        
        Vector3d offset = corner.pos;
        Vector3d temp;
        if ((states & (1 << controller.f_id))&&(states & (1 << controller.b_id)))
        {
            if (blockCanFall(&area, &corner))
            {
                setVector3d(&offset, corner.pos.x, corner.pos.y, corner.pos.z - 1);
                setBlockPosition(&corner, offset);
            }
        }
        else
        {
        if (states & (1 << controller.f_id))
        {
            if (!(old_states & (1 << controller.f_id)))
            {
                temp = offset;
                setVector3d(&temp, corner.pos.x - 1, corner.pos.y, corner.pos.z);
                setBlockPosition(&corner, temp);

                if (!blockOverlapsArea(&area, &corner))
                    offset = temp;

                //setVector3d(&offset, corner.pos.x - 1, corner.pos.y, corner.pos.z);
                setBlockPosition(&corner, offset);
            }
        }
        if (states & (1 << controller.l_id))
        {
            if (!(old_states & (1 << controller.l_id)))
            {
                temp = offset;
                setVector3d(&temp, corner.pos.x, corner.pos.y - 1, corner.pos.z);               
                setBlockPosition(&corner, temp);

                if (!blockOverlapsArea(&area, &corner))
                    offset = temp;

                //setVector3d(&offset, corner.pos.x, corner.pos.y - 1, corner.pos.z);
                setBlockPosition(&corner, offset);
            }
        }
        if (states & (1 << controller.b_id))
        {
            if (!(old_states & (1 << controller.b_id)))
            {
                temp = offset;
                setVector3d(&temp, corner.pos.x + 1, corner.pos.y, corner.pos.z);
                setBlockPosition(&corner, temp);

                if (!blockOverlapsArea(&area, &corner))
                    offset = temp;

                //setVector3d(&offset, corner.pos.x + 1, corner.pos.y, corner.pos.z);
                setBlockPosition(&corner, offset);
            }
        }
        if (states & (1 << controller.r_id))
        {
            if (!(old_states & (1 << controller.r_id)))
            {
                temp = offset;
                setVector3d(&temp, corner.pos.x, corner.pos.y + 1, corner.pos.z);
                setBlockPosition(&corner, temp);

                if (!blockOverlapsArea(&area, &corner))
                    offset = temp;

                //setVector3d(&offset, corner.pos.x, corner.pos.y + 1, corner.pos.z);
                setBlockPosition(&corner, offset);
            }
        }
        }

        if (states & (1 << controller.u_id))
        {
            if (!(old_states & (1 << controller.u_id)))
            {
                Tritris temp_block = corner;
                rotateBlock(&temp_block, 2);

                if (!blockOverlapsArea(&area, &temp_block))
                    corner = temp_block;

                //rotateBlock(&corner, 2);
            }
        }
        if (states & (1 << controller.d_id))
        {
            if (!(old_states & (1 << controller.d_id)))
            {
                Tritris temp_block = corner;
                rotateBlock(&temp_block, 0);
        
                if (!blockOverlapsArea(&area, &temp_block))
                    corner = temp_block;

                //rotateBlock(&corner, 0);        
            }
        }
        
        old_states = states;

        if (delay_cnt >= 20)
        {
            //if (!(corner.pos.z == 0)) 
            if (blockCanFall(&area, &corner))
            {
                setVector3d(&offset, corner.pos.x, corner.pos.y, corner.pos.z - 1);
                setBlockPosition(&corner, offset);
            }
            else
            {
                new_block = 1;
                fillTritrisArea(&area, &corner);

                uint8_t block_val = checkTritrisArea(framebuffer, &area);
                if (block_val)
                {
                    //score = 10;
                    block_val++;
                }
                score += block_val*block_val;
                if (checkAreaForGameOver(&area))
                    break;
            }
            delay_cnt = 0;
        }

        clearFrameBuffer_(framebuffer);
        drawTritrisArea(framebuffer, &area);
        drawTritrisBlock(framebuffer, &corner);
        frameBufferToBits_DB_NEW(framebuffer);
        //delay_ms(delay);
        delay_cnt++;
    }
    //Flash cube to signify game over
    for (int i=0; i<5; i++)
    {
        setFrameBuffer(framebuffer);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(8*25);

        clearFrameBuffer_(framebuffer);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(8*25);
    }
    //Tally points
    tallyScore_anm(framebuffer, score, 1, 50);
}

void powerTest(FrameBuffer *framebuffer, int delay)
{
    //Driven fully
    setFrameBuffer(framebuffer);
    frameBufferToBits_DB(framebuffer); 
    delay_ms(delay);

    //Fully off
    clearFrameBuffer_(framebuffer);
    frameBufferToBits_DB(framebuffer);
    delay_ms(delay);

    //Bottom half layers
    clearFrameBuffer_(framebuffer);
    for (int i=0; i<FBUF_SZ/2; i++)
        drawPlane(framebuffer, XY_PLANE, i);
    //drawPlane(framebuffer, XY_PLANE, 0);
    //drawPlane(framebuffer, XY_PLANE, 1);
    frameBufferToBits_DB(framebuffer);
    delay_ms(delay);

    //Top half layers
    clearFrameBuffer_(framebuffer);
    for (int i=FBUF_SZ/2; i<FBUF_SZ; i++)
         drawPlane(framebuffer, XY_PLANE, i);
    //drawPlane(framebuffer, XY_PLANE, 2);
    //drawPlane(framebuffer, XY_PLANE, 3);
    frameBufferToBits_DB(framebuffer);
    delay_ms(delay);

    //Left Half layers
    clearFrameBuffer_(framebuffer);
    for (int i=0; i<FBUF_SZ/2; i++)
        drawPlane(framebuffer, XZ_PLANE, i);
    //drawPlane(framebuffer, XZ_PLANE, 0);
    //drawPlane(framebuffer, XZ_PLANE, 1);
    frameBufferToBits_DB(framebuffer);
    delay_ms(delay);

    //Right half layers
    clearFrameBuffer_(framebuffer);
    for (int i=FBUF_SZ/2; i<FBUF_SZ; i++)
        drawPlane(framebuffer, XZ_PLANE, i);
    //drawPlane(framebuffer, XZ_PLANE, 2);
    //drawPlane(framebuffer, XZ_PLANE, 3);
    frameBufferToBits_DB(framebuffer);
    delay_ms(delay);
}


void mainLoop(FrameBuffer *framebuffer, int delay)
{
    ButtonController controller;
    initButtonController(&controller);

    uint8_t selection = 1;
    uint8_t sub_selection = 1;
    uint8_t sel_max = 10;
    uint8_t sub_sel_max = 5;

    uint8_t states;
    uint8_t old_states = getButtonStates(&controller);

    uint8_t blink = 0;

    while (1)
    {
        states = getButtonStates(&controller);
        int step = 2;
        for (int j=0; j<delay/step; j++)
        {
            delay_ms(step);
            states |= getButtonStates(&controller);
        }

        if ((states & (1 << controller.f_id)) && (!(old_states & (1 << controller.f_id))))
        {
            if (sub_selection < sub_sel_max)
                sub_selection++;    
        }
        if ((states & (1 << controller.l_id)) && (!(old_states & (1 << controller.l_id))))
        {
            if (selection > 1)
                selection--;
            else
                selection = sel_max-1;

            sub_selection = 1;
        }
        if ((states & (1 << controller.b_id)) && (!(old_states & (1 << controller.b_id))))
        {
            if (sub_selection > 1)
                sub_selection--;
        }
        if ((states & (1 << controller.r_id)) && (!(old_states & (1 << controller.r_id))))
        {
            if (selection < sel_max)
                selection++;
            else
                selection = 1;
            sub_selection = 1;
        }


        if ((states & (1 << controller.u_id)) && (!(old_states & (1 << controller.u_id))))
        {
            switch (selection)
            {
                case 1:
                {   
                    shiftPlane_anm(framebuffer, sub_selection - 1, 1, delay);
                    break;
                }
                case 2:
                {
                    rotatePlane_anm(framebuffer, sub_selection - 1, delay);
                    break;
                }
                case 3:
                {
                    snake_anm(framebuffer, sub_selection, delay);
                    break;
                }
                case 4:
                {
                    delay_ms(250);
                    snakeGame(framebuffer, sub_selection*delay);
                    break;
                }
                case 5:
                {
                    tritrisGame(framebuffer, sub_selection*(delay/4));
                    break;
                }
                case 6:
                {
                    powerTest(framebuffer, sub_selection*delay*5);
                    break;
                }
            }   
        }


        old_states = states;

        clearFrameBuffer_(framebuffer);
        if (blink < 128)
        {
            drawBinaryNumber(framebuffer, selection, 1, 2);
            drawBinaryNumber(framebuffer, sub_selection, 1, 0);
        }
        frameBufferToBits_DB(framebuffer);


        //Resolve new max selections 
        switch (selection)
        {
            case 1://Shift plane. Axis control
            {
                //shiftPlane_anm(FrameBuffer *framebuffer, uint8_t axis, int delay);
                sub_sel_max = 3;
                break;
            }
            case 3://Snake Animation. Loop number control
            {
                sub_sel_max = 5;
                break;
            }
            case 4://Snake game. Game speed control
            {
                sub_sel_max = 5;
            }
            case 5://Tritris game. Game speed control
            {
                sub_sel_max = 5;
                break;
            }
            case 6:
            {
                sub_sel_max = 10;
                break;
            }
            default:
            {
                sub_sel_max = 3;
                break;
            }
        }
        blink += 30; 
    }
}





/*
//Functions you'll probably never use
void drawBuffer()
{
    bufferToBits();
    uint8_t copy[2];
    for (int i=0; i<2; i++)
        copy[i] = bufferBits[cnt][i];
    loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, copy, 16);
}
void drawBufferQuick()
{
    bufferToBitsQuick();
    loadShiftRegister_(REG_CLK, LATCH_CLK, DATA_PIN, bufferBits[cnt], 16);
}
i*/
