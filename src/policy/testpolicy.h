#ifndef TESTPOLICY_H
#define TESTPOLICY_H

/*
 * Stupid-simple test policy
 * for controlling robot.
 *
 */

#include "build/boxbot.pb.h"
#include "policy.h"

using boxbot::ControlData;
using boxbot::ObservationData;

class TestPolicy : public Policy
{
public:
    TestPolicy(uint xdim, uint udim);
    const ControlData computeControls(const ObservationData &data);
};

#endif // TESTPOLICY_H
