#ifndef SIM_H
#define SIM_H

#include <Box2D/Box2D.h>
//#include "debugdraw.h"

class Agent;

class Sim
{
public:
    Sim();
    ~Sim();
    void step();
    b2World* getWorld();
    Agent* getAgent(int i);
    void addAgent(Agent *agent);

protected:
    std::vector<Agent*> m_agents;

    b2World* m_world;

    //DebugDraw m_debugDraw;
    float m_globaltime;

    // deprecated features
    int load_proto(const char *filename);
};

#endif // SIM_H
