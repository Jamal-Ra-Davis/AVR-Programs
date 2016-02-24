#pragma once
#include <stdint.h>
#define FBUF_SZ 6
#define FBUF_REGS 5


#define XZ_PLANE 0
#define YZ_PLANE 1
#define XY_PLANE 2

typedef struct
{
    uint8_t size;
    uint8_t buffer[FBUF_SZ][FBUF_SZ][FBUF_SZ];
    uint8_t bufferBits[FBUF_SZ][FBUF_REGS];
    uint8_t currLevel;
} FrameBuffer;

FrameBuffer* makeFrameBuffer();
void initFrameBuffer(FrameBuffer* framebuffer);
void frameBufferToBits(FrameBuffer* framebuffer);
void frameBufferToBits_DB(FrameBuffer* framebuffer);
void frameBufferToBits_DB_NEW(FrameBuffer* framebuffer);

//Drawing functions
void clearFrameBuffer(FrameBuffer *framebuffer);//Clears framebuffer and sets buffer bits
void clearFrameBuffer_(FrameBuffer *framebuffer);//Just clears framebuffer
void setFrameBuffer(FrameBuffer *framebuffer);//Sets entire frame buffer
void setPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z);//doesn't set buffer bits
void clearPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z);
void togglePixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z);

void drawLine(FrameBuffer *framebuffer, uint8_t axis, uint8_t x, uint8_t y);
void drawPlane(FrameBuffer *framebuffer, uint8_t axis, uint8_t level);
void clearPlane(FrameBuffer *framebuffer, uint8_t axis, uint8_t level);

void drawBinaryNumber(FrameBuffer *framebuffer, uint16_t number, uint8_t axis, uint8_t level);

void shiftPlane_anm(FrameBuffer *framebuffer, uint8_t axis, int delay);
void rotatePlane_anm(FrameBuffer *framebuffer, uint8_t axis, int delay);
void snake_anm(FrameBuffer *framebuffer, uint8_t loops, int delay);
void tallyScore_anm(FrameBuffer *framebuffer, int score, uint8_t axis, int delay);

void controlPixel(FrameBuffer *framebuffer, int delay);
void snakeGame(FrameBuffer *framebuffer, int delay);
void tritrisGame(FrameBuffer *framebuffer, int delay);

void powerTest(FrameBuffer *framebuffer, int delay);

void mainLoop(FrameBuffer *framebuffer, int delay);
