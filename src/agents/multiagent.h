#ifndef MULTIAGENT_H
#define MULTIAGENT_H

// Agent comprised of multiple "sub-agents"
// for instance, multiple octoArms working in tandem
#include "agent.h"
#include <vector>

class MultiAgent : public Agent
{
public:
    MultiAgent();
    ~MultiAgent();
    // add child agent
    void addAgent(Agent *agent);
    void step(float time);
protected:
    std::vector<Agent *> m_agents;
};

#endif // MULTIAGENT_H
