#include "Tritris.h"
#include "Vector3d.h"
#include "FrameBuffer.h"
#include "delay.h"
#include <stdlib.h>

Tritris* makeTritrisBlock(uint8_t type, uint8_t color)
{
    Tritris *out = (Tritris*)malloc(sizeof(Tritris));
    initTritrisBlock(out, type, color);
    return out;
}
void initTritrisBlock(Tritris* block, uint8_t type, uint8_t color)
{
    switch (type)
    {
        case 1:
        {
            setVector3d(&(block->blocks[0]), 0, 1, 0);
            setVector3d(&(block->blocks[1]), 0, 0, 0);
            setVector3d(&(block->blocks[2]), 1, 0, 0);
            break;
        }
        default://Line block is default, if come up with more, add +1 cases
        {
            setVector3d(&(block->blocks[0]), -1, 0, 0);
            setVector3d(&(block->blocks[1]), 0, 0, 0);
            setVector3d(&(block->blocks[2]), 1, 0, 0);
            break;
        }
    }

    setVector3d(&(block->pos), 0, 0, 0);

    block->theta = 0;
    block->phi = 0;
    block->psi = 0;

    block->color = color;
}
void setBlockPosition(Tritris* block, Vector3d pos_)
{
    setVector3d(&(block->pos), pos_.x, pos_.y, pos_.z);
}
void setBlockColor(Tritris* block, uint8_t color)
{
    block->color = color;
}   
//maybe add increment poistion for each axiz functions
void rotateBlock(Tritris* block, int axis)//Postive axis numbers rotate 90-deg in that dir, negaitve rotate -90
{
    switch (axis)
    {
        case 0:
        {
            block->psi++;
            if (block->psi >= 4)
                block->psi = 0;
            break;
        }
        case 1:
        {
            block->phi++;
            if (block->phi >= 4)
                block->phi = 0;
            break;
        }
        case 2:
        {
            block->theta++;
            if (block->theta >= 4)
                block->theta = 0;
            break;
        }
    }
    
}
Vector3d calcRotation(int index, Tritris *block)
{
    Vector3d out;
    setVector3d(&out, block->blocks[index].x, block->blocks[index].y, block->blocks[index].z);
    for (int i=0; i<block->psi; i++)
    {
        setVector3d(&out, -1*out.z, out.y, out.x);
    }
    for (int i=0; i<block->phi; i++)
    {
        setVector3d(&out, out.x, -1*out.z, out.y);
    }
    for (int i=0; i<block->theta; i++)
    {
        setVector3d(&out, -1*out.y, out.x, out.z);
    }
    return out;
}
void drawTritrisBlock(FrameBuffer* framebuffer, Tritris* block)
{
    for (int i=0; i<BLOCK_SZ; i++)
    {
        Vector3d finalPos = calcRotation(i, block);
        finalPos = addVector3d(finalPos, block->pos);
    
        //drawVector3d(framebuffer, finalPos);
        setPixelColor(framebuffer, finalPos.x, finalPos.y, finalPos.z, block->color);
    }   
}
void copyBlock(Tritris* new_block, Tritris* old_block)
{
    new_block->pos = old_block->pos;
    new_block->theta = old_block->theta;
    new_block->phi = old_block->phi;
    new_block->psi = old_block->psi;

    new_block->color = old_block->color;

    for (int i=0; i<BLOCK_SZ; i++)
        new_block->blocks[i] = old_block->blocks[i];
    
}


TritrisArea* makeTritrisArea()
{
    TritrisArea* tritrisArea= (TritrisArea*)malloc(sizeof(TritrisArea));
    initTritrisArea(tritrisArea);
    return tritrisArea;
}
void initTritrisArea(TritrisArea* gameArea)
{
    for (int k=0; k<AREA_HEIGHT; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                gameArea->gameArea[k][i][j] = 0;
            }
        }
    }
}
void fillTritrisArea(TritrisArea* gameArea, Tritris* block)
{
    for (int i=0; i<BLOCK_SZ; i++)
    {
        Vector3d finalPos = calcRotation(i, block);
        finalPos = addVector3d(finalPos, block->pos);
        if ((finalPos.x >= 0)&&(finalPos.y >= 0)&&(finalPos.z >= 0)&&(finalPos.x < FBUF_SZ)&&(finalPos.y < FBUF_SZ)&&(finalPos.z < 8))
            gameArea->gameArea[finalPos.z][finalPos.x][finalPos.y] = block->color;
    }
}
void drawTritrisArea(FrameBuffer* framebuffer, TritrisArea* gameArea)
{
    for (int k=0; k<FBUF_SZ; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                if (gameArea->gameArea[k][i][j])
                    setPixelColor(framebuffer, i, j, k, gameArea->gameArea[k][i][j]);
            }
        }
    }
}

uint8_t blockOverlapsArea(TritrisArea* gameArea, Tritris* block)
{
    for (int i=0; i<BLOCK_SZ; i++)
    {
        Vector3d finalPos = calcRotation(i, block);
        finalPos = addVector3d(finalPos, block->pos);
        if ((finalPos.z < 0)||(finalPos.x < 0)||(finalPos.y < 0))
            return 1;
        if ((finalPos.x >= FBUF_SZ)||(finalPos.y >= FBUF_SZ))
            return 1;

        if (finalPos.z < AREA_HEIGHT)
        {
            if (gameArea->gameArea[finalPos.z][finalPos.x][finalPos.y] > 0)
                return 1;
        }
    }
    return 0;
}
uint8_t blockCanFall(TritrisArea* gameArea, Tritris* block)
{
    Tritris new_block = *block;
    //copyBlock(&new_block, block);
    setVector3d(&(new_block.pos), new_block.pos.x, new_block.pos.y, new_block.pos.z-1);
    uint8_t res = blockOverlapsArea(gameArea, &new_block); 
    if (res)
        return 0;
    else
        return 1;
}

uint8_t checkTritrisArea(FrameBuffer* framebuffer, TritrisArea* gameArea)
{
/*
    clearFrameBuffer_(framebuffer);
    for (int i=0; i<4; i++)
    {
        drawPlane(framebuffer, 2, i);
        frameBufferToBits_DB(framebuffer);
        delay_ms(250);
    }
    for (int i=0; i<4; i++)
    {
        clearPlane(framebuffer, 2, i);
        frameBufferToBits_DB(framebuffer);
        delay_ms(250);
    }
*/

    //Determine which levels are full
    //uint8_t levels_filled = 0x0F;

    uint8_t filled_levels[AREA_HEIGHT];
    for (int i=0; i<AREA_HEIGHT; i++)
        filled_levels[i] = 1;

    uint8_t levels_filled = 0xFF;
    uint8_t levels_cleared = 0;
    for (int k=0; k<AREA_HEIGHT; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                if (!gameArea->gameArea[k][i][j])
                {
                    levels_filled &= ~(1 << k);
                    filled_levels[k] = 0;
                }
            }
        }
    }
    for (int i=0; i<AREA_HEIGHT; i++)
    {
        //if (levels_filled & (1 << i))
        //    levels_cleared++; 
        if (filled_levels[i])
            levels_cleared++;
    }

    //levels_filled = 0x03;
    uint8_t fullPlanesCheck = 0;
    for (int i=0; i<AREA_HEIGHT-4; i++)
    {
        if (filled_levels[i])
            fullPlanesCheck = 1;
    }


    //if (levels_filled & 0x0F)
    if (fullPlanesCheck)
    {
        int delay = 8*150;
        //Flash cleared levels
        for (int i=0; i<4; i++)
        {
            //clearFrameBuffer_(framebuffer);
            for (int k=0; k<FBUF_SZ; k++)
            {
                //if (levels_filled & (1 << k))
                if (filled_levels[k])
                    clearPlane(framebuffer, 2, k);
            }
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        
            //clearFrameBuffer_(framebuffer);
            for (int k=0; k<FBUF_SZ; k++)
            {
                //if (levels_filled & (1 << k))
                if (filled_levels[k])
                    drawPlane(framebuffer, 2, k);
            }
            frameBufferToBits_DB_NEW(framebuffer);
            delay_ms(delay);
        }

        while(1)
        {
            uint8_t levelsRemain = 0;
            for (int k=0; k<AREA_HEIGHT; k++)
            {
                uint8_t levelFilled = 1;
                for (int i=0; i<FBUF_SZ; i++)
                {
                    for (int j=0; j<FBUF_SZ; j++)
                    {
                        if (!gameArea->gameArea[k][i][j])
                            levelFilled = 0;
                    }
                }
                if (levelFilled)
                {
                    for (int x=k; x<AREA_HEIGHT; x++)
                    {
                        for (int i=0; i<FBUF_SZ; i++)
                        {
                            for (int j=0; j<FBUF_SZ; j++)
                            {
                                if (x == (AREA_HEIGHT-1))
                                {
                                    gameArea->gameArea[x][i][j] = 0;
                                }
                                else
                                {
                                    gameArea->gameArea[x][i][j] = gameArea->gameArea[x+1][i][j];
                                }
                            }
                        }
                    }
                    levelsRemain = 1;
                    break;
                }
            }
            if (!levelsRemain)
                break;
        }
    }
    return levels_cleared;
    //return 0;
}
uint8_t checkAreaForGameOver(TritrisArea* gameArea)
{
    for (int k=AREA_HEIGHT-4; k<AREA_HEIGHT; k++)
    {
        for (int i=0; i<FBUF_SZ; i++)
        {
            for (int j=0; j<FBUF_SZ; j++)
            {
                if (gameArea->gameArea[k][i][j])
                    return 1;
            }
        }
    }
    return 0;
}
