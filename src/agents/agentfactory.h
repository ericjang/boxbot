#ifndef AGENTFACTORY_H
#define AGENTFACTORY_H

//void make_octoarm
#include <Box2D/Box2D.h>
#include "mathutils.h"

class Agent;
class MultiAgent;

// creates a tentacle appendage
Agent * makeTentacle(b2World *world,
                    b2Body *base,
                    xform_t lx,
                    int n_segments);

Agent *makePolypAgent(b2World *world, xform_t xf);
Agent *makeOctoArmAgent(b2World *world, xform_t xf);
Agent *makeWalkerAgent(b2World *world, xform_t xform);

#endif // AGENTFACTORY_H
