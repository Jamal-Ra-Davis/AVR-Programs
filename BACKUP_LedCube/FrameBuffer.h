#pragma once
#define FBUF_SZ 4

typedef struct
{
    uint8_t size;
    uint8_t buffer[FBUF_SZ][FBUF_SZ][FBUF_SZ];
    uint8_t bufferBits[FBUF_SZ][2];
    uint8_t currLevel;
} FrameBuffer;

FrameBuffer* makeFrameBuffer();
void initFrameBuffer(FrameBuffer* framebuffer);
void frameBufferToBits(FrameBuffer* framebuffer);

//Drawing functions
void clearFrameBuffer(FrameBuffer *framebuffer);//Clears framebuffer and sets buffer bits
void clearFrameBuffer_(FrameBuffer *framebuffer);//Just clears framebuffer
void setPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z);//doesn't set buffer bits
void clearPixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z);
void togglePixel(FrameBuffer *framebuffer, uint8_t x, uint8_t y, uint8_t z);





