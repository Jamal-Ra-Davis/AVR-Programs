#include <stdlib.h>
#include <avr/io.h>
#include "Snake.h"
#include "Vector3d.h"
#include "FrameBuffer.h"

Snake* makeSnake(uint8_t x, uint8_t y, uint8_t z)
{
    Snake *snake = (Snake*)malloc(sizeof(Snake));
    initSnake(snake, x, y, z);
    return snake;
}
void initSnake(Snake *snake, uint8_t x, uint8_t y, uint8_t z)
{
    snake->body_len = BODY_LEN_INIT;
    //for (int i=0; i<BODY_LEN; i++)
    for (int i=0; i<snake->body_len; i++)
    {
        setVector3d((snake->body + i), x, y, z);
    }
}
void setHead(Snake *snake, uint8_t x, uint8_t y, uint8_t z)
{
    Vector3d newPoint;
    setVector3d(&newPoint, x, y, z);
    //for (int i=BODY_LEN-1; i>=1; i--)
    for (int i=snake->body_len-1; i>=1; i--)
    {
        snake->body[i] = snake->body[i-1];
    }
    snake->body[0] = newPoint;
};
//Other functions will be responsible for clearing buffer
void drawSnake(FrameBuffer *framebuffer, Snake *snake)
{
    //for (int i=0; i<BODY_LEN; i++)
    for (int i=0; i<snake->body_len; i++)
    {
        //setPixel(framebuffer, (uint8_t)snake->body[i].x, (uint8_t)snake->body[i].y);
        drawVector3d(framebuffer, snake->body[i]);
    }
}
int snakeCrossesBody(Snake *snake)
{
    //for (int i=1; i<BODY_LEN; i++)
    for (int i=1; i<snake->body_len; i++)
    {
        if ((snake->body[0].x == snake->body[i].x)&&
            (snake->body[0].y == snake->body[i].y)&&
            (snake->body[0].z == snake->body[i].z))
            return 1;
    }
    return 0;
}
int pointOverlapsSnake(Snake *snake, Vector3d point)
{
    //for (int i=0; i<BODY_LEN; i++)
    for (int i=0; i<snake->body_len; i++)
    {
        if ((point.x == snake->body[i].x)&&
            (point.y == snake->body[i].y)&&
            (point.z == snake->body[i].z))
            return 1;
    }
    return 0;
}
void incrementSnake(Snake *snake)
{
    if (snake->body_len >= BODY_LEN)
        return;
    snake->body_len++;

    /*
    snake->body[snake->body_len-1].x = snake->body[snake->body_len-2].x;
    snake->body[snake->body_len-1].x = snake->body[snake->body_len-2].x;
    snake->body[snake->body_len-1].x = snake->body[snake->body_len-2].x;
    */
    snake->body[snake->body_len-1] = snake->body[snake->body_len-2];
}


