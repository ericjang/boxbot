#include "testpolicy.h"
#include <math.h>

using boxbot::ControlData;
using boxbot::ObservationData;


TestPolicy::TestPolicy(uint xdim, uint udim)
    : Policy(xdim, udim)
{
}

const ControlData TestPolicy::computeControls(const ObservationData &data)
{
    ControlData cdata;
    for (int i=0; i<m_udim; i++)
    {
        float u = (i%2==1) ? 20.*sin(data.time()) : 0.;
        cdata.add_control_data(u);
    }
    return cdata;
}
