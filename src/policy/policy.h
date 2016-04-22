#ifndef POLICY_H
#define POLICY_H

#include "build/boxbot.pb.h"

/*
 * Abstract policy class that takes in observations
 * and returns controls for the robot.
 */

using boxbot::ControlData;
using boxbot::ObservationData;

class Policy
{
public:
    Policy(uint x_dim, uint u_dim);
    // given const reference
    virtual const ControlData computeControls(const ObservationData& data) = 0;
protected:
    uint m_xdim;
    uint m_udim;
};

#endif // POLICY_H
