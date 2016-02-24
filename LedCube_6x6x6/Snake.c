#include <stdlib.h>
#include <avr/io.h>
#include "Snake.h"
#include "Vector3d.h"
#include "FrameBuffer.h"
#include "delay.h"

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
        snake->bodyColor[i] = WHITE;
        setVector3d((snake->body + i), x, y, z);
        //setVector3d((&snake->body_[i].pos), x, y, z);
        
        //snake->body_[i].color = WHITE;
    }

}
void snakeInitTest(FrameBuffer *framebuffer, Snake *snake)
{
    for (int i=0; i<3; i++)
    {
        setFrameBufferColor(framebuffer, WHITE);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(1500);
        clearFrameBuffer_(framebuffer);
        frameBufferToBits_DB_NEW(framebuffer);
        delay_ms(1500);
    }
}
void setHead(Snake *snake, uint8_t x, uint8_t y, uint8_t z)
{
    Vector3d newPoint;
    setVector3d(&newPoint, x, y, z);

    //SnakeBody newBodyPiece;
    //newBodyPiece.pos = newPoint;
    //for (int i=BODY_LEN-1; i>=1; i--)
    for (int i=snake->body_len-1; i>=1; i--)
    {
        snake->body[i] = snake->body[i-1];
        //snake->body_[i] = snake->body_[i-1];
    }
    snake->body[0] = newPoint;
    //snake->body_[0] = newBodyPiece;
};
//Other functions will be responsible for clearing buffer
void drawSnake(FrameBuffer *framebuffer, Snake *snake)
{
    //for (int i=0; i<BODY_LEN; i++)
    for (int i=0; i<snake->body_len; i++)
    {
        uint8_t test_color = WHITE;
        //snake->body_[i].color = WHITE;
        //drawVector3d(framebuffer, snake->body[i]);
        //setPixelColor(framebuffer, (uint8_t)snake->body_[i].pos.x, (uint8_t)snake->body_[i].pos.y, (uint8_t)snake->body_[i].pos.z, WHITE/*(uint8_t)(snake->bodyColor[i])*/);
        //setPixelColor(framebuffer, (uint8_t)snake->body[i].x, (uint8_t)snake->body[i].y, (uint8_t)snake->body[i].z, snake->body_[i].color);
        setPixelColor(framebuffer, (uint8_t)snake->body[i].x, (uint8_t)snake->body[i].y, (uint8_t)snake->body[i].z, /*WHITE*/snake->bodyColor[i]);
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
        //if ((snake->body_[0].pos.x == snake->body_[i].pos.x)&&
        //    (snake->body_[0].pos.y == snake->body_[i].pos.y)&&
        //    (snake->body_[0].pos.z == snake->body_[i].pos.z))
        //    return 1;
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
        //if ((point.x == snake->body_[i].pos.x)&&
        //    (point.y == snake->body_[i].pos.y)&&
        //    (point.z == snake->body_[i].pos.z))
        //    return 1;
    }
    return 0;
}
void incrementSnake(Snake *snake, uint8_t color)
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
    snake->bodyColor[snake->body_len-1] = color;   
    //snake->body_[snake->body_len-1] = snake->body_[snake->body_len-2];
    //snake->body_[snake->body_len-1].color = color;
}


