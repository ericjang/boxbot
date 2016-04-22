#include "mathutils.h"

xform_t getXform(b2Body *body)
{
    return xform(body->GetPosition().x, body->GetPosition().y, body->GetAngle());
}

xform_t toGlobal(xform_t ref, xform_t local)
{
    xform_t g;
    float t=ref.a;

    g.x = ref.x + local.x*cos(t) + local.y*cos(t+b2_pi/2);
    g.y = ref.y + local.x*sin(t) + local.y*sin(t+b2_pi/2);
    //g.x = ref.x + local.x*cos(a) + local.y*cos(b2_pi/2-a);
    //g.y = ref.y + local.x*sin(a) + local.y*sin(b2_pi/2-a);
    g.a = ref.a+local.a;
    return g;
}
