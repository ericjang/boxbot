#include "envfactory.h"
#include "box2dhelper.h"
#include "mathutils.h"

void makeWallFixture(b2World *world, xform_t xform, b2Vec2 s)
{
    b2Body *b=makeStaticBody(world, xform);
    b2PolygonShape shape=makeBox(s);
    makeFixture(b,&shape);
}

void makeEnv1(b2World *world)
{
    b2Vec2 s(0.1,5); // half-width, half-height
    makeWallFixture(world, xform(0,0,0), b2Vec2(5,.1)); // floor
    makeWallFixture(world, xform(-5,2,0), b2Vec2(.1,2)); // left wall
    makeWallFixture(world, xform(5,2,0), b2Vec2(.1,2)); // right wall
}
