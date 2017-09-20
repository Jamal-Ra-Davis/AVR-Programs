#include <avr/io.h>

#include "RingBuffer.h"

void initRingBuf(RingBuffer *ringbuffer)
{
    ringbuffer->cnt = 0;
    ringbuffer->start = 0;
    ringbuffer->end = 0;
    ringbuffer->error = 0;
    ringbuffer->isEmpty = 1;
    ringbuffer->isFull = 0;
}

void pushRingBuf(RingBuffer *ringbuffer, uint8_t val)
{
    ringbuffer->error = 0;
    if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt > 0))
    {
        ringbuffer->error = RBUF_ERROR_OFLW;
        return;
    }
    ringbuffer->buffer[ringbuffer->end] = val;
    ringbuffer->end++;
    ringbuffer->cnt++;
    if (ringbuffer->end == RING_BUFSZ)
        ringbuffer->end = 0;


    if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt > 0))
        ringbuffer->isFull = 1;
    else
        ringbuffer->isFull = 0;

    if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt == 0))
        ringbuffer->isEmpty = 1;
    else
        ringbuffer->isEmpty = 0;
}

uint8_t popRingBuf(RingBuffer *ringbuffer)
{
    ringbuffer->error = 0;
    if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt == 0))
    {
        ringbuffer->error = RBUF_ERROR_UFLW;
        return 0;
    }
    uint8_t val = ringbuffer->buffer[ringbuffer->start];
    ringbuffer->start++;
    ringbuffer->cnt--;
    if (ringbuffer->start == RING_BUFSZ)
        ringbuffer->start = 0;


    if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt > 0))
        ringbuffer->isFull = 1;
    else
        ringbuffer->isFull = 0;
    if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt == 0))
        ringbuffer->isEmpty = 1;
    else
        ringbuffer->isEmpty = 0;


    return val;
}
uint8_t peekRingBuf(RingBuffer *ringbuffer)
{
	ringbuffer->error = 0;
	if ((ringbuffer->start == ringbuffer->end)&&(ringbuffer->cnt == 0))
    {
        ringbuffer->error = 2;
        return 0;
    }
	return ringbuffer->buffer[ringbuffer->start];
}
