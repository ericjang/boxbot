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

    for (int i=0; i<agent->getJoints().size(); i++)
        m_joints.push_back(agent->getJoints().at(i));
    // just inherit all the joints for now
    //copy(agent->getJoints().begin(), agent->getJoints().end(), std::back_inserter(m_joints));
}

#if 0
// better approach - execute children in distributed manner
void MultiAgent::step(float time)
{

    for (int i=0; i<m_agents.size(); i++)
    {
        Agent *agent=m_agents.at(i);
        agent->step(time);
    }
}
#endif
