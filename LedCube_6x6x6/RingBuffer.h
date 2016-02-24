#pragma once
#include <avr/io.h>

#define RING_BUFSZ 64

typedef struct {
    uint8_t cnt;
    uint8_t start;
    uint8_t end;
    uint8_t buffer[RING_BUFSZ];

    uint8_t error;
    uint8_t isEmpty;
    uint8_t isFull;

} RingBuffer;

void initRingBuf(RingBuffer *ringbuffer);
void pushRingBuf(RingBuffer *ringbuffer, uint8_t val);
uint8_t popRingBuf(RingBuffer *ringbuffer);

