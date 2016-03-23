#pragma once

#include "FrameBuffer.h"

typedef struct
{
    int x, y, z;
} Vector3d;

Vector3d* makeVector3d(int x_, int y_, int z_);
void setVector3d(Vector3d *point, int x_, int y_, int z_);
void drawVector3d(FrameBuffer *framebuffer, Vector3d point);
Vector3d addVector3d(Vector3d A, Vector3d B);
Vector3d subtractVector(Vector3d final, Vector3d initial);
