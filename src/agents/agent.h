
#ifndef AGENT_H
#define AGENT_H

#include <Box2D/Box2D.h>
#include <vector>
#include "build/boxbot.pb.h"

using boxbot::ControlData;

class Policy;
/**
 * @brief Agent embodied in a world (Box2D for now, we
 * can abstract the world interface later)
 *
 */
class Agent
{
    friend class Policy;

public:
    Agent();
    ~Agent();
    //void load_proto(const ::boxbot::AgentDef& ad);

    // call loadAgent or buildAgent
    //void loadAgentProto(); // load in a body defined via protocol buffer
    //void buildAgent();

    // take observation and apply controls encoded in Observation protobuf x
    void step(float time);
    void addJoint(b2Joint *joint);
    void addBody(b2Body *body);
    std::vector<b2Body *> getBodies();
    std::vector<b2Joint*> getJoints();
    void setPolicy(Policy* p);

protected:
    std::vector<b2Body*> m_bodies;
    std::vector<b2Joint*> m_joints;
    Policy *m_policy;

    // specific to each robot
    virtual void applyControls(const ControlData& data);

};

#endif // AGENT_H
