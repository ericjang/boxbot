#ifndef BOX2DHELPER_H
#define BOX2DHELPER_H

#include <Box2D/Box2D.h>
#include "mathutils.h"

// bodies
b2Body *makeStaticBody(b2World *world, xform_t xform);
b2Body *makeDynamicBody(b2World *world, xform_t xform);

// shapes
b2CircleShape makeCircle(float r);
b2EdgeShape makeEdge(b2Vec2 v1, b2Vec2 v2);
b2PolygonShape makeBox(b2Vec2 size);

// joints
b2Joint *makePrismaticJoint(b2World* world, b2Body* a, b2Body *b);
b2Joint *makeRevoluteJoint(b2World* world, b2Body *a, b2Body *b, b2Vec2 anchor);

// fixtures
void makeFixture(b2Body *body, b2Shape *shape);

// convenience
b2Body* makeAxle(b2World* world, b2Body* base, xform_t xform);

#endif // BOX2DHELPER_H
