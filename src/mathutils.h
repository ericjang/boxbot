#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <math.h>
#include <Box2D/Box2D.h>

typedef struct xform
{
    float x;
    float y;
    float a; // angle, radians
    xform() : x(0), y(0), a(0) {}
    xform(float x, float y, float a) : x(x), y(y), a(a) {}
} xform_t;

xform_t getXform(b2Body *body);

xform_t toGlobal(xform_t ref, xform_t local);

#endif // MATHUTILS_H
