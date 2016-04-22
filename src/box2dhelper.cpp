#include "box2dhelper.h"
#include "mathutils.h"
/*
 * Convenience fns for making various box2D shapes
 */

// BODY FUNCTIONS

b2Body *makeStaticBody(b2World *world, xform_t xform)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(xform.x,xform.y);
    bodyDef.angle=xform.a;
    return world->CreateBody(&bodyDef);
}

b2Body *makeDynamicBody(b2World *world, xform_t xform)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(xform.x,xform.y);
    bodyDef.angle=xform.a;
    // other useful params set here (angular damping?)
    bodyDef.angularDamping=1.;
    bodyDef.linearDamping=1.;
    return world->CreateBody(&bodyDef);
}

// SHAPE FUNCTIONS

b2CircleShape makeCircle(float r)
{
    b2CircleShape shape;
    shape.m_radius=r;
    return shape;
}

b2EdgeShape makeEdge(b2Vec2 v1, b2Vec2 v2)
{
    // v1, v2 are relative to body
    b2EdgeShape shape;
    shape.Set(v1,v2);
    return shape;
}

b2PolygonShape makeBox(b2Vec2 size)
{
    b2PolygonShape shape;
    shape.SetAsBox(size.x, size.y);
    return shape;
}

// Joint functions

b2Joint* makePrismaticJoint(b2World* world, b2Body* a, b2Body *b)
{
   // make prismatic joint between 2 bodies
   b2PrismaticJointDef jointDef;
   b2Vec2 worldAxis(1.0,0.0);
   jointDef.Initialize(a,b,a->GetWorldCenter(), worldAxis);
   jointDef.lowerTranslation = -5.f;
   jointDef.upperTranslation = 5.f; // muscles can only contract
   jointDef.enableLimit = true;
   jointDef.maxMotorForce = 100.0f;
   jointDef.motorSpeed = 0.0f;
   jointDef.enableMotor = true;
   return world->CreateJoint(&jointDef);
}

b2Joint *makeRevoluteJoint(b2World* world, b2Body *a, b2Body *b, b2Vec2 anchor)
{
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(a, b, anchor);
    jointDef.lowerAngle = -0.5f * b2_pi; // -45 degrees
    jointDef.upperAngle = 0.5f * b2_pi; // 45 degrees
    jointDef.enableLimit = true;
    return world->CreateJoint(&jointDef);
}

// fixtures

void makeFixture(b2Body *body, b2Shape *shape)
{
    b2FixtureDef fDef;
    fDef.shape=shape;
    fDef.density=1.0f;
    fDef.friction=1.0f;
    body->CreateFixture(&fDef);
}

b2Body* makeAxle(b2World* world, b2Body* base, xform_t xform)
{
    // creates a small square axle body and a revolute joint centered at the anchor
    b2Body * axle = makeDynamicBody(world, xform);
    b2CircleShape shape = makeCircle(.04);
    makeFixture(axle, &shape);
    makeRevoluteJoint(world, base, axle, b2Vec2(xform.x, xform.y));
    return axle;
}

