#pragma once
#include "RingBuffer.h"
#include "FrameBuffer.h"

typedef struct
{
    RingBuffer *ringbuffer;

    uint8_t mode, anm_mode;
    
    uint8_t message_type;
    uint8_t anm_type, anm_idx;
    uint8_t color, axis, delay, loops;
    int valid_message;

}MessageHandler;


int initMessageHandler(MessageHandler* messageHandler, RingBuffer* ringBuffer);
int handleAnimationMessage(MessageHandler* messageHandler, FrameBuffer* framebuffer);
int handleDrawingMessage(MessageHandler* messageHandler, FrameBuffer* framebuffer);
