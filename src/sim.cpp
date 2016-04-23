#include "sim.h"
#include <stdio.h>
#include "agents/agent.h"

#include "envfactory.h"

#include "box2dhelper.h"
#include "policy/rpcclientpolicy.h"

#include "policy/testpolicy.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
//#include <grpc++/grpc++.h>
#include "build/boxbot.grpc.pb.h"

#define DRAW_STRING_NEW_LINE 25

Sim::Sim()
{
    m_globaltime=0.f;


//    uint32 flags = 0;
//    flags += 1			* b2Draw::e_shapeBit;
//    flags += 1			* b2Draw::e_jointBit;
//    flags += 1 * b2Draw::e_aabbBit;
//    flags += 1 * b2Draw::e_pairBit;
//    flags += 1 * b2Draw::e_centerOfMassBit;
//    flags += 1 * b2Draw::e_particleBit;

//    m_debugDraw.SetFlags(flags);
//    m_world->SetDebugDraw(&m_debugDraw);


    b2Vec2 gravity(0,-9.8);
    m_world=new b2World(gravity);
    makeEnv1(m_world);



    // make default world


    // make agent
    //Agent * agent = makeOctoArmAgent(m_world, xform(0,4,0));
    //Agent * agent = makeWalkerAgent(m_world, xform(0,3,0));
    //Agent *agent = makePolypAgent(m_world, xform(0,0.3, b2_pi/2));

    // make policy
    //Policy *policy = new TestPolicy(0, agent->getJoints().size());
    //Policy *policy = new RPCPolicy(0, agent->getJoints().size(),grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials()));
    //agent->setPolicy(policy);
}

Sim::~Sim()
{
    delete m_world;
    for (int i=0; i<m_agents.size(); i++)
        delete m_agents.at(i);
}

b2World *Sim::getWorld()
{
    return m_world;
}

Agent *Sim::getAgent(int i)
{
    return m_agents.at(i);
}

void Sim::addAgent(Agent *agent)
{
    m_agents.push_back(agent);
}

void Sim::step()
{
    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (60Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.
    const float32 timeStep = 1.0f / 60.0f;
    const int32 velocityIterations = 6;
    const int32 positionIterations = 2;
    const int32 particleIterations = 2;
    // int m_textLine=DRAW_STRING_NEW_LINE;

    m_globaltime+=timeStep;

#if 0
    for(auto it = m_agents.begin(); it != m_agents.end(); ++it) {
        (*it)->step(m_globaltime);
    }
#endif

    m_world->Step(timeStep, velocityIterations, positionIterations, particleIterations);

    //m_debugDraw.
    //m_debugDraw.DrawString(5, m_textLine, "boxbot-viz"); // draws in pink!
    //m_textLine+=DRAW_STRING_NEW_LINE;

    //m_world->DrawDebugData();

//    m_debugDraw.DrawString(5, m_textLine, "particles/groups/pairs/triads = %d/%d/%d/%d", particleCount, groupCount, pairCount, triadCount);
//    m_textLine += DRAW_STRING_NEW_LINE;

    // Now print the position and angle of the body.
//    b2Vec2 position = body->GetPosition();
//    float32 angle = body->GetAngle();
//    printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);

}
