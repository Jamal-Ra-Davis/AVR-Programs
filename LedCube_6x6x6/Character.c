#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include "Character.h"
#include "Vector3d.h"
#include "FrameBuffer.h"

uint8_t *getCharacterData(char c)
{
    if (c >= 65 && c <= 90)
        return characters[c-65];
    
    if (c >= 48 && c <= 57)
        return numbers[c-48];

    return NULL;
/*
    if (c < 65 || c > 90)
        return NULL;

    return characters[c-65];
*/
}

Character* makeCharacter(uint8_t x, uint8_t y, uint8_t z, uint8_t color, uint8_t *data_ptr)
{
    Character *character  = (Character*)malloc(sizeof(Character));
    initCharacter(character, x, y, z, color, data_ptr);
    return character;
}
void initCharacter(Character *character, uint8_t x, uint8_t y, uint8_t z, uint8_t color, uint8_t *data_ptr)
{
    //setVector3d(&(character->pos), x, y, z);
    setCharacterPosition(character, x, y, z);
    //character->color = color; 
    setCharacterColor(character, color);
    //character->character_data = data_ptr;
    character->character_data = NULL;
    setCharacterData(character, data_ptr);
}
void setCharacterPosition(Character *character, uint8_t x, uint8_t y, uint8_t z)
{
    setVector3d(&(character->pos), x, y, z);
}
void setCharacterColor(Character *character, uint8_t color)
{
    character->color = color;
}
void setCharacterData(Character *character, uint8_t *data_ptr)
{
    character->character_data = data_ptr;
}
void drawCharacter(FrameBuffer *framebuffer, Character *character)
{
    if (character->character_data == NULL)
    {
        //setFrameBufferColor(framebuffer, character->color);
        return;
    }
    for (int i=0; i<FBUF_SZ; i++)
    {
        for (int j=0; j<FBUF_SZ; j++)
        {
            if (character->character_data[FBUF_SZ-1-i] & (1 << (5-j)))
                setPixelColor(framebuffer, character->pos.x, character->pos.y + j, character->pos.z + i, character->color);
        }
    }
}

Marquee* makeMarquee(int offset, char* text, uint8_t text_color)
{
    Marquee *marquee  = (Marquee*)malloc(sizeof(Marquee));
    initMarquee(marquee, offset, text, text_color);
    return marquee;
}
void initMarquee(Marquee* marquee, int offset, char* text, uint8_t text_color)
{
    marquee->offset = offset;
    marquee->text_size = strlen(text);
    if (marquee->text_size > MARQUEE_MAX)
        marquee->text_size = MARQUEE_MAX;
    
    for (int i=0; i<marquee->text_size; i++)
    {
        initCharacter(marquee->text+i, 0, 0, 0, text_color, getCharacterData(text[i]));
    } 
    for (int i=0; i<3; i++)
    {
        marquee->control[i] = 0xFF;
    }   
    marquee->control[2] = 0x00;
}
Vector3d marqueePosToVector(uint8_t pos)//Store error code in z value
{
    uint8_t lookup_table[20][2] = {
                                    {0, 0},
                                    {0, 1},
                                    {0, 2},
                                    {0, 3},
                                    {0, 4},
                                    {0, 5},
                                    {1, 5},
                                    {2, 5},
                                    {3, 5},
                                    {4, 5},
                                    {5, 5},
                                    {5, 4},
                                    {5, 3},
                                    {5, 2},
                                    {5, 1},
                                    {5, 0},
                                    {4, 0},
                                    {3, 0},
                                    {2, 0},
                                    {1, 0},
                                  };
    Vector3d vec;
    if (pos >= 20 || pos < 0)
    {
        setVector3d(&vec, 0, 0, -1);
        return vec;
    }
    setVector3d(&vec, lookup_table[pos][0], lookup_table[pos][1], 0);
    return vec;
}
int marqueeEndIdx(Marquee* marquee)
{
    int CHAR_SZ = 6;
    return CHAR_SZ*marquee->text_size+(4*FBUF_SZ-4); 
}
void drawMarquee(FrameBuffer *framebuffer, Marquee *marquee)
{
    int CHAR_SZ = 6;
    //int pos = 0;
   
    for (int pos=0; pos<20; pos++)
    { 
        //int curr_offset = marquee->offset;
        for (int i=0; i<marquee->text_size; i++)
        {
            if (pos < (marquee->offset + CHAR_SZ*(i+1)))
            {
                int start = marquee->offset + CHAR_SZ*i;
                int idx = pos - start;
                if (marquee->text[marquee->text_size - 1 - i].character_data != NULL)
                {
                    Vector3d vec = marqueePosToVector((uint8_t)pos);
                    if (vec.z == 0)
                    {
                        for (int j=0; j<FBUF_SZ; j++)
                        {
                            int idx_ = CHAR_SZ - 1 - idx;
                            if (marquee->text[marquee->text_size - 1 - i].character_data[FBUF_SZ-1-j] & (1 << idx_))
                                setPixelColor(framebuffer, vec.x, vec.y, j, marquee->text[marquee->text_size - 1 - i].color);
                        }
                    }
                }
            }
        }
    }
}

void drawMarquee_alt(FrameBuffer *framebuffer, Marquee *marquee)
{
    int MARQUEE_AREA = 20;//FBUF_SZ*2 + (FBUF_SZ-2)*2;;
    int CHAR_SZ = 6;
    int pos;
    for (int i=0; i<marquee->text_size; i++)
    {
        if (marquee->text[i].character_data == NULL)
            continue;
        for (int j=0; j<CHAR_SZ; j++)
        {
            //pos = -1*(i*(CHAR_SZ-1) + j) + marquee->offset - 1;
            pos = -1*(CHAR_SZ*i + j) +  marquee->offset - 1;//New calculation to fix spacing issue
            if (pos < 0)
                return;
            if (pos < MARQUEE_AREA)
            {
                uint8_t c_idx, c_pos;
                c_idx = pos/8;
                c_pos = pos % 8;
                if (!(marquee->control[c_idx] & (0x80 >> c_pos)))
                    continue;
                Vector3d vec = marqueePosToVector((uint8_t)pos);
                if (vec.z != 0)//Failed to get vector
                    continue;
                for (int k=0; k<FBUF_SZ; k++)
                {
                    if (marquee->text[i].character_data[FBUF_SZ-1-k] & (0x20 >> j))
                        setPixelColor(framebuffer, vec.x, vec.y, k, marquee->text[i].color);
                }
            }
        }
    }
}
