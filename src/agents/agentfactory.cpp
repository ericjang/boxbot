#include "agentfactory.h"
#include "agent.h"
#include "multiagent.h"
#include "mathutils.h"
#include "box2dhelper.h"

const float hx=.3;
const float hy=.5;

b2Joint* makeMuscle(b2World *world, b2Body *pbody, b2Body *cbody, xform_t p_attach, xform_t c_attach)
{
    b2Body *p_axle = makeAxle(world, pbody, p_attach);
    b2Body *c_axle = makeAxle(world, cbody, c_attach);
    return makePrismaticJoint(world, p_axle, c_axle);
}

// untransformed pose is horizontal, from left to right
Agent *makeTentacle(b2World *world, xform_t xf, int n_segments)
{
    Agent *agent = new Agent();

    // xf is the global xform, convert it to local
    // for iteration
    xform_t px=xform(0,0,0);
    xform_t lx=xf;

    b2Body *pbody;
    b2Body *cbody;

    b2Vec2 psize(0,hy*2);
    b2Vec2 csize;

    for (int i=0; i<n_segments; i++)
    {
        // create child segment
        xform_t cx = toGlobal(px, lx); // global xform of child body
        cbody = makeDynamicBody(world, cx);
        agent->addBody(cbody);
        csize = b2Vec2(hx,hy*(1.-i*.1));
        b2PolygonShape cshape=makeBox(csize);
        makeFixture(cbody, &cshape);

        if (i>0)
        {
            // attachment child body to parent
            xform_t attach = toGlobal(cx, xform(-hx*.9, 0, 0)); // attachment point of child
            makeRevoluteJoint(world, pbody, cbody, b2Vec2(attach.x, attach.y));
            // add muscles
            for (int j=0; j<2; j++)
            {
                int k = j*2-1; // -1, 1 for dorsal / ventral sides
                xform_t p_attach = toGlobal(px, xform(0,k*.8*psize.y,0));
                xform_t c_attach = toGlobal(cx, xform(0,k*.8*csize.y,0));
                agent->addJoint(makeMuscle(world,pbody,cbody,p_attach,c_attach));
            }
        }

        // new segment becomes next parent segment
        pbody=cbody;
        // subsequent segments have fixed xform relative to previous segment
        px=cx;
        psize=csize;
        lx=xform(3./2*hx, 0, 0);
    }

    return agent;
}

Agent *makeOctoArmAgent(b2World *world, xform_t xf)
{
    // fixed to a static
    b2Body *pivot=makeStaticBody(world, xf);
    b2CircleShape shape=makeCircle(.01);
    makeFixture(pivot,&shape);

    // make tentacle
    xform_t xf2 = toGlobal(xf, xform(0.75,0,0));
    Agent *agent = makeTentacle(world, xf2, 6);

    // make rotary base
    b2Body *base=makeDynamicBody(world, xf);
    shape=makeCircle(.5);
    makeFixture(base,&shape);
    b2Joint * joint = makeRevoluteJoint(world, pivot, base, b2Vec2(xf.x, xf.y));
    agent->addJoint(joint);

    // fix tentacle to base via revolute joint
    b2Body *cbody = agent->getBodies().at(0);
    xform_t cxf = getXform(cbody);
    xform_t anchor = toGlobal(cxf, xform(-hx,0,0));
    makeRevoluteJoint(world, base, cbody, b2Vec2(anchor.x, anchor.y));

    // add muscles between base and tentacle
    xform_t p_attach_t = toGlobal(xf, xform(0.3,.3,0));
    xform_t p_attach_b = toGlobal(xf, xform(.3,-.3,0));

    xform_t c_attach_t = toGlobal(cxf,xform(-hx/2,hy,0));
    xform_t c_attach_b = toGlobal(cxf,xform(-hx/2,-hy,0));

    agent->addJoint(makeMuscle(world, base, cbody, p_attach_t, c_attach_t));
    agent->addJoint(makeMuscle(world, base, cbody, p_attach_b, c_attach_b));

    return agent;
}

Agent *makePolypAgent(b2World *world, xform_t xf)
{
    Agent *agent=makeTentacle(world, xf, 6);

    b2Body *cbody = agent->getBodies().at(0);

    // fixed to static
    b2Body *pivot=makeStaticBody(world, xf);
    makeRevoluteJoint(world, pivot, cbody, b2Vec2(xf.x, xf.y));
    return agent;
}

Agent *makeWalkerAgent(b2World *world, xform_t xf)
{
    // xform - global xform

    // central body
    MultiAgent *agent = new MultiAgent();

    b2Body *main_body = makeDynamicBody(world, xf);
    agent->addBody(main_body);
    b2PolygonShape body_shape=makeBox(b2Vec2(.8,.8));
    makeFixture(main_body, &body_shape);

    for (int i=0; i<2; i++) // for each leg
    {
        // pivot point of leg
        int k=2*i-1; // -1,1
        xform_t xf_leg = toGlobal(xf, xform(k*.6,-.8, (1.25+.5*i)*b2_pi));
        Agent *leg = makeTentacle(world, xf_leg, 5);
        b2Body *legRoot = leg->getBodies().at(0);
        makeRevoluteJoint(world,main_body,legRoot,b2Vec2(xf_leg.x, xf_leg.y));

        // attach muscles
        xform_t cxf = getXform(legRoot);
        xform_t p_attach_t = toGlobal(cxf, xform(-1.2*hx,1.2*hy,0));
        xform_t p_attach_b = toGlobal(cxf, xform(-1.2*hx,-1.2*hy,0));

        xform_t c_attach_t = toGlobal(cxf,xform(-hx/2,hy,0));
        xform_t c_attach_b = toGlobal(cxf,xform(-hx/2,-hy,0));

        leg->addJoint(makeMuscle(world, main_body, legRoot, p_attach_t, c_attach_t));
        leg->addJoint(makeMuscle(world, main_body, legRoot, p_attach_b, c_attach_b));

        agent->addAgent(leg);
    }

    return agent;
}
