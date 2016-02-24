#include "Tritris.h"
#include "Vector3d.h"
#include "FrameBuffer.h"
#include "delay.h"
#include <stdlib.h>

Tritris* makeTritrisBlock(uint8_t type)
{
    Tritris *out = (Tritris*)malloc(sizeof(Tritris));
    initTritrisBlock(out, type);
    return out;
}
void initTritrisBlock(Tritris* block, uint8_t type)
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
}
void setBlockPosition(Tritris* block, Vector3d pos_)
{
    setVector3d(&(block->pos), pos_.x, pos_.y, pos_.z);
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
    
        drawVector3d(framebuffer, finalPos);
    }   
}
void copyBlock(Tritris* new_block, Tritris* old_block)
{
    new_block->pos = old_block->pos;
    new_block->theta = old_block->theta;
    new_block->phi = old_block->phi;
    new_block->psi = old_block->psi;

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
    for (int k=0; k<8; k++)
    {
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
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
        if ((finalPos.x >= 0)&&(finalPos.y >= 0)&&(finalPos.z >= 0)&&(finalPos.x < 4)&&(finalPos.y < 4)&&(finalPos.z < 8))
            gameArea->gameArea[finalPos.z][finalPos.x][finalPos.y] = 1;
    }
}
void drawTritrisArea(FrameBuffer* framebuffer, TritrisArea* gameArea)
{
    for (int k=0; k<4; k++)
    {
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                if (gameArea->gameArea[k][i][j])
                    setPixel(framebuffer, i, j, k);
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
        if ((finalPos.x >= 4)||(finalPos.y >= 4))
            return 1;

        if (finalPos.z < 8)
        {
            if (gameArea->gameArea[finalPos.z][finalPos.x][finalPos.y] == 1)
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
    uint8_t levels_filled = 0xFF;
    uint8_t levels_cleared = 0;
    for (int k=0; k<8; k++)
    {
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                if (!gameArea->gameArea[k][i][j])
                {
                    levels_filled &= ~(1 << k);
                }
            }
        }
    }
    for (int i=0; i<8; i++)
    {
        if (levels_filled & (1 << i))
            levels_cleared++; 
    }

    //levels_filled = 0x03;
    if (levels_filled & 0x0F)
    {
        int delay = 150;
        //Flash cleared levels
        for (int i=0; i<4; i++)
        {
            //clearFrameBuffer_(framebuffer);
            for (int k=0; k<4; k++)
            {
                if (levels_filled & (1 << k))
                    clearPlane(framebuffer, 2, k);
            }
            frameBufferToBits_DB(framebuffer);
            delay_ms(delay);
        
            //clearFrameBuffer_(framebuffer);
            for (int k=0; k<4; k++)
            {
                if (levels_filled & (1 << k))
                    drawPlane(framebuffer, 2, k);
            }
            frameBufferToBits_DB(framebuffer);
            delay_ms(delay);
        }

        while(1)
        {
            uint8_t levelsRemain = 0;
            for (int k=0; k<8; k++)
            {
                uint8_t levelFilled = 1;
                for (int i=0; i<4; i++)
                {
                    for (int j=0; j<4; j++)
                    {
                        if (!gameArea->gameArea[k][i][j])
                            levelFilled = 0;
                    }
                }
                if (levelFilled)
                {
                    for (int x=k; x<8; x++)
                    {
                        for (int i=0; i<4; i++)
                        {
                            for (int j=0; j<4; j++)
                            {
                                if (x == 7)
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
    for (int k=4; k<8; k++)
    {
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                if (gameArea->gameArea[k][i][j])
                    return 1;
            }
        }
    }
    return 0;
}
