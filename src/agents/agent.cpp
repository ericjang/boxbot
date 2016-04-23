#include "agent.h"
#include "box2dhelper.h"
#include "policy/rpcclientpolicy.h"
#include <vector>

Agent::Agent()
{

}

Agent::~Agent()
{
    delete m_policy;
}

void Agent::step(float t)
{
    // compute controls and apply them

    ObservationData data;
    data.set_time(t);
    ControlData cdata = m_policy->computeControls(data);
    applyControls(cdata);
}

void Agent::applyControls(const ControlData& data)
{
    assert(data.control_data_size() == m_joints.size());
    for (int i=0; i<m_joints.size(); i+=1)
    {
        b2Joint *joint = m_joints.at(i);
        float u=data.control_data(i);

        if (joint->GetType()==b2JointType::e_revoluteJoint)
        {
            b2RevoluteJoint *rjoint = (b2RevoluteJoint*)m_joints.at(i);
            rjoint->SetMotorSpeed(u);
        }
        else if (joint->GetType()==b2JointType::e_prismaticJoint)
        {
            b2PrismaticJoint *pjoint = (b2PrismaticJoint*)m_joints.at(i);
            pjoint->SetMotorSpeed(u);
            //joint->SetMaxMotorForce(u); // force in newtons
        }

    }
}

void Agent::addJoint(b2Joint *joint)
{
    m_joints.push_back(joint);
}

void Agent::addBody(b2Body *body)
{
    m_bodies.push_back(body);
}

std::vector<b2Body*> Agent::getBodies()
{
    return m_bodies;
}

std::vector<b2Joint*> Agent::getJoints()
{
    return m_joints;
}

void Agent::setPolicy(Policy *p)
{
    m_policy=p;
}
