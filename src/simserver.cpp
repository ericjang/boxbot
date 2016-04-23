#include "simserver.h"
#include <string>
#include <grpc++/grpc++.h>
#include "sim.h"
#include "agents/agent.h"
#include "agents/agentfactory.h"
#include "debugdraw.h"

#include "GL/freeglut.h"

// GRPC STUFF

using grpc::Status;
using grpc::Server;
using grpc::ServerContext;
using boxbot::ObservationData;
using boxbot::ControlData;
using boxbot::ExperimentDef;
using boxbot::SimParams;


RPCSimImpl::RPCSimImpl(Sim *sim, void (*f1)(const boxbot::ExperimentDef *), void (*f2)())
    : m_sim(sim), m_fn_initSim(f1), m_fn_redraw(f2)
{

}

Status RPCSimImpl::init(ServerContext *context, const ExperimentDef *edef, SimParams *sp)
{
    std::cout << "received init request" << std::endl;
    m_fn_initSim(edef);

    // set SimParams
    sp->set_u_dim(12);
    // TODO - fixme
    sp->set_x_dim(100 * 100);

    return Status::OK;
}

Status RPCSimImpl::step(ServerContext *context, const ControlData *cdata, ObservationData *odata)
{
    // just support the first agent for now
    Agent *agent = m_sim->getAgent(0);
    agent->applyControls(*cdata);
    // integrate
    m_sim->step();

    m_fn_redraw();

    // TODO - get back observation
    // can we just do glReadPixels here?
    //observe();

    // return back observation image
    odata->set_float_data(0,0);

    return Status::OK;
}
