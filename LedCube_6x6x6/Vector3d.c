#include <stdlib.h>
#include <avr/io.h>
#include "Vector3d.h"
//#include "FrameBuffer.h"

Vector3d* makeVector3d(int x_, int y_, int z_)
{
    Vector3d *point = (Vector3d*)malloc(sizeof(Vector3d));
    setVector3d(point, x_, y_, z_);
    return point;
}
void setVector3d(Vector3d *point, int x_, int y_, int z_)
{
    point->x = x_;
    point->y = y_;
    point->z = z_;
}
//void drawVector3d(FrameBuffer *framebuffer, Vector3d point)
//{
//    setPixel(framebuffer, (uint8_t)point.x, (uint8_t)point.y, (uint8_t)point.z);
//}
Vector3d addVector3d(Vector3d A, Vector3d B)
{
    Vector3d out;
    setVector3d(&out, A.x + B.x, A.y + B.y, A.z + B.z);
    return out;
}
Vector3d subtractVector(Vector3d final, Vector3d initial)
{
    Vector3d out;
    setVector3d(&out, final.x - initial.x, final.y - initial.y, final.z - initial.z);
    return out;
}
