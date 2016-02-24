#pragma once
#define BODY_LEN 10
#define BODY_LEN_INIT 3
#include "FrameBuffer.h"
#include "Vector3d.h"

typedef struct
{
    Vector3d body[BODY_LEN];
    uint8_t body_len;
} Snake;

Snake* makeSnake(uint8_t x, uint8_t y, uint8_t z);
void initSnake(Snake *snake, uint8_t x, uint8_t y, uint8_t z);
void setHead(Snake *snake, uint8_t x, uint8_t y, uint8_t z);
void drawSnake(FrameBuffer *framebuffer, Snake *snake);
int snakeCrossesBody(Snake *snake);
int pointOverlapsSnake(Snake *snake, Vector3d point);
void incrementSnake(Snake *snake);

