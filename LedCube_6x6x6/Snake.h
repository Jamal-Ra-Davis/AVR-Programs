#pragma once
#define BODY_LEN 20
#define BODY_LEN_INIT 5
#include "FrameBuffer.h"
#include "Vector3d.h"

typedef struct
{
    Vector3d pos;
    uint8_t color;
} SnakeBody;
typedef struct
{
    Vector3d body[BODY_LEN];
    uint8_t bodyColor[BODY_LEN];

    uint8_t body_len;    

    //SnakeBody body_[BODY_LEN];
} Snake;


Snake* makeSnake(uint8_t x, uint8_t y, uint8_t z);
void initSnake(Snake *snake, uint8_t x, uint8_t y, uint8_t z);
void snakeInitTest(FrameBuffer *framebuffer, Snake *snake);

void setHead(Snake *snake, uint8_t x, uint8_t y, uint8_t z);
void drawSnake(FrameBuffer *framebuffer, Snake *snake);
int snakeCrossesBody(Snake *snake);
int pointOverlapsSnake(Snake *snake, Vector3d point);
void incrementSnake(Snake *snake, uint8_t);

