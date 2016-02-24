#pragma once
#include <stdint.h>
#include "Vector3d.h"
#include "FrameBuffer.h"
#define BLOCK_SZ 3
#define AREA_HEIGHT 10

typedef struct
{
    Vector3d blocks[BLOCK_SZ];

    Vector3d pos;

    uint8_t theta, phi, psi;
    uint8_t color;

} Tritris;

typedef struct
{   
    uint8_t gameArea[AREA_HEIGHT][FBUF_SZ][FBUF_SZ];
} TritrisArea;


Tritris* makeTritrisBlock(uint8_t type, uint8_t color);
void initTritrisBlock(Tritris* block, uint8_t type, uint8_t color);
void setBlockPosition(Tritris* block, Vector3d pos_);
void setBlockColor(Tritris* block, uint8_t color);
//maybe add increment poistion for each axiz functions
void rotateBlock(Tritris* block, int axis);//Rotates +90 is specified axis
Vector3d calcRotation(int index, Tritris *blocks);
void drawTritrisBlock(FrameBuffer* framebuffer, Tritris* block);
void copyBlock(Tritris* new_block, Tritris* old_block);
uint8_t checkBlockRange(Tritris* block, uint8_t axis, int lower, int upper);

TritrisArea* makeTritrisArea();
void initTritrisArea(TritrisArea* gameArea);
void fillTritrisArea(TritrisArea* gameArea, Tritris* block);
void drawTritrisArea(FrameBuffer* framebuffer, TritrisArea* gameArea);

uint8_t blockOverlapsArea(TritrisArea* gameArea, Tritris* block);
uint8_t blockCanFall(TritrisArea* gameArea, Tritris* block);

uint8_t checkTritrisArea(FrameBuffer* framebuffer, TritrisArea* gameArea); //Returns number of levels cleared
uint8_t checkAreaForGameOver(TritrisArea* gameArea);
