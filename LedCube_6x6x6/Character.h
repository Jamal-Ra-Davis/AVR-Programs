#pragma once
#include "Vector3d.h"
#include "FrameBuffer.h"

#define MARQUEE_MAX 64
/*
static uint8_t characters[26][FBUF_SZ] = 
    {
        {   //A
            0b000000,
            0b001100,
            0b010010,
            0b011110,
            0b010010,
            0b010010
        },
        {   //B
            0b000000,
            0b011100,
            0b010010,
            0b011100,
            0b010010,
            0b011100
        },
        {   //C
            0b000000,
            0b011110,
            0b010000,
            0b010000,
            0b010000,
            0b011110
        },
        {   //D
            0b000000,
            0b011100,
            0b010010,
            0b010010,
            0b010010,
            0b011100
        },
        {   //E
            0b000000,
            0b011110,
            0b010000,
            0b011110,
            0b010000,
            0b011110
        },
        {   //F
            0b000000,
            0b011110,
            0b010000,
            0b011110,
            0b010000,
            0b010000
        },
        {   //G
            0b000000,
            0b001100,
            0b010000,
            0b010110,
            0b010010,
            0b001100
        },
        {   //H
            0b000000,
            0b010010,
            0b010010,
            0b011110,
            0b010010,
            0b010010
        },
        {   //I
            0b000000,
            0b011110,
            0b001100,
            0b001100,
            0b001100,
            0b011110
        },
        {   //J
            0b000000,
            0b000010,
            0b000010,
            0b000010,
            0b010010,
            0b001100
        },
        {   //K
            0b000000,
            0b010010,
            0b010100,
            0b011000,
            0b010100,
            0b010010
        },
        {   //L
            0b000000,
            0b010000,
            0b010000,
            0b010000,
            0b010000,
            0b011110
        },
        {   //M
            0b000000,
            0b100001,
            0b110011,
            0b101101,
            0b100001,
            0b100001
        },
        {   //N
            0b000000,
            0b110001,
            0b101001,
            0b100101,
            0b100011,
            0b100001
        },
        {   //O
            0b000000,
            0b001100,
            0b010010,
            0b010010,
            0b010010,
            0b001100
        },
        {   //P
            0b000000,
            0b011100,
            0b010010,
            0b011100,
            0b010000,
            0b010000
        },
        {   //Q
            0b000000,
            0b001100,
            0b010010,
            0b010010,
            0b010110,
            0b001100
        },
        {   //R
            0b000000,
            0b011100,
            0b010010,
            0b011100,
            0b010100,
            0b010010
        },
        {   //S
            0b000000,
            0b011110,
            0b010000,
            0b011110,
            0b000010,
            0b011110
        },
        {   //T
            0b000000,
            0b011110,
            0b001100,
            0b001100,
            0b001100,
            0b001100
        },
        {   //U
            0b000000,
            0b010010,
            0b010010,
            0b010010,
            0b010010,
            0b001100
        },
        {   //V
            0b000000,
            0b100001,
            0b100001,
            0b010010,
            0b010010,
            0b001100
        },
        {   //W
            0b000000,
            0b100001,
            0b100001,
            0b101101,
            0b010010,
            0b010010
        },
        {   //X
            0b000000,
            0b010010,
            0b010010,
            0b001100,
            0b010010,
            0b010010
        },
        {   //Y
            0b000000,
            0b010010,
            0b010010,
            0b011110,
            0b001100,
            0b001100
        },
        {   //Z
            0b000000,
            0b011110,
            0b000010,
            0b001100,
            0b010000,
            0b011110
        }
    };
*/
static uint8_t characters[26][FBUF_SZ] =
    {
        {   //A
            0b000000,
            0b111110,
            0b100010,
            0b111110,
            0b100010,
            0b100010
        },
        {   //B
            0b000000,
            0b111110,
            0b100010,
            0b101100,
            0b100010,
            0b111110
        },
        {   //C
            0b000000,
            0b111110,
            0b100000,
            0b100000,
            0b100000,
            0b111110
        },
        {   //D
            0b000000,
            0b111100,
            0b100010,
            0b100010,
            0b100010,
            0b111100
        },
        {   //E
            0b000000,
            0b111110,
            0b100000,
            0b111100,
            0b100000,
            0b111110
        },
        {   //F
            0b000000,
            0b111110,
            0b100000,
            0b111100,
            0b100000,
            0b100000
        },
        {   //G
            0b000000,
            0b111110,
            0b100000,
            0b100110,
            0b100010,
            0b111110
        },
        {   //H
            0b000000,
            0b100010,
            0b100010,
            0b111110,
            0b100010,
            0b100010
        },
        {   //I
            0b000000,
            0b111110,
            0b001000,
            0b001000,
            0b001000,
            0b111110
        },
        {   //J
            0b000000,
            0b000110,
            0b000010,
            0b000010,
            0b100010,
            0b111110
        },
        {   //K
            0b000000,
            0b100010,
            0b100100,
            0b111000,
            0b100100,
            0b100010
        },
        {   //L
            0b000000,
            0b100000,
            0b100000,
            0b100000,
            0b100000,
            0b111110
        },
        {   //M
            0b000000,
            0b100010,
            0b110110,
            0b101010,
            0b100010,
            0b100010
        },
        {   //N
            0b000000,
            0b100010,
            0b110010,
            0b101010,
            0b100110,
            0b100010
        },        
        {   //O
            0b000000,
            0b011100,
            0b100010,
            0b100010,
            0b100010,
            0b011100
        },
        {   //P
            0b000000,
            0b111100,
            0b100010,
            0b111100,
            0b100000,
            0b100000
        },
        {   //Q
            0b000000,
            0b111110,
            0b100010,
            0b100010,
            0b111110,
            0b001000
        },
        {   //R
            0b000000,
            0b111100,
            0b100010,
            0b111100,
            0b100010,
            0b100010
        },
        {   //S
            0b000000,
            0b111110,
            0b100000,
            0b111110,
            0b000010,
            0b111110
        },
        {   //T
            0b000000,
            0b111110,
            0b001000,
            0b001000,
            0b001000,
            0b001000
        },
        {   //U
            0b000000,
            0b100010,
            0b100010,
            0b100010,
            0b100010,
            0b111110
        },
        {   //V
            0b000000,
            0b100010,
            0b100010,
            0b010100,
            0b010100,
            0b001000
        },
        {   //W
            0b000000,
            0b100010,
            0b100010,
            0b101010,
            0b101010,
            0b010100
        },
        {   //X
            0b000000,
            0b100010,
            0b010100,
            0b001000,
            0b010100,
            0b100010
        },
        {   //Y
            0b000000,
            0b100010,
            0b100010,
            0b010100,
            0b001000,
            0b001000
        },
        {   //Z
            0b000000,
            0b111110,
            0b000100,
            0b001000,
            0b010000,
            0b111110
        }
    };
static uint8_t numbers[10][FBUF_SZ] =
    {
        {   //0
            0b000000,
            0b111110,
            0b100110,
            0b101010,
            0b110010,
            0b111110
        },
        {   //1
            0b000000,
            0b001000,
            0b011000,
            0b001000,
            0b001000,
            0b011100
        },
        {   //2
            0b000000,
            0b111100,
            0b000010,
            0b011100,
            0b100000,
            0b111110
        },
        {   //3
            0b000000,
            0b111110,
            0b000010,
            0b011100,
            0b000010,
            0b111110
        },
        {   //4
            0b000000,
            0b100000,
            0b101000,
            0b101000,
            0b111110,
            0b001000
        },
        {   //5
            0b000000,
            0b111110,
            0b100000,
            0b111100,
            0b000010,
            0b111100
        },
        {   //6
            0b000000,
            0b111110,
            0b100000,
            0b111110,
            0b100010,
            0b111110
        },
        {   //7
            0b000000,
            0b111110,
            0b000010,
            0b000100,
            0b001000,
            0b001000
        },
        {   //8
            0b000000,
            0b111110,
            0b100010,
            0b111110,
            0b100010,
            0b111110
        },
        {   //9
            0b000000,
            0b111110,
            0b100010,
            0b111110,
            0b000010,
            0b111110
        }
    };



typedef struct
{
    uint8_t color;
    uint8_t *character_data;
    Vector3d pos;
} Character;
typedef struct
{
    int offset;
    Character text[MARQUEE_MAX];
    uint8_t text_size;
    
    uint8_t control[3];//4*FBUF_SZ - 4
}Marquee;



uint8_t *getCharacterData(char c);

Character* makeCharacter(uint8_t x, uint8_t y, uint8_t z, uint8_t color, uint8_t *data_ptr);
void initCharacter(Character *character, uint8_t x, uint8_t y, uint8_t z, uint8_t color, uint8_t *data_ptr);
void setCharacterPosition(Character *character, uint8_t x, uint8_t y, uint8_t z);
void setCharacterColor(Character *character, uint8_t color);
void setCharacterData(Character *character, uint8_t *data_ptr);

void drawCharacter(FrameBuffer *framebuffer, Character *character);


Marquee* makeMarquee(int offset, char* text, uint8_t text_color);
void initMarquee(Marquee* marquee, int offset, char* text, uint8_t text_color);
Vector3d marqueePosToVector(uint8_t pos);//Store error code in z value
int marqueeEndIdx(Marquee* marquee);

void drawMarquee(FrameBuffer *framebuffer, Marquee *marquee);
void drawMarquee_alt(FrameBuffer *framebuffer, Marquee *marquee);
