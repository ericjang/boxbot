#include "multiagent.h"
#include "agent.h"

MultiAgent::MultiAgent()
{
}

MultiAgent::~MultiAgent()
{
    // free child agents
    for (int i=0; i<m_agents.size(); i++)
        delete m_agents.at(i);
}

void MultiAgent::addAgent(Agent *agent)
{
    m_agents.push_back(agent);
}

void MultiAgent::step(float time)
{
    // for now, execute children in distributed manner
    for (int i=0; i<m_agents.size(); i++)
    {
        Agent *agent=m_agents.at(i);
        agent->step(time);
    }
}
