#ifndef SIMSERVER_H
#define SIMSERVER_H

/*
 * (optionally headless) BoxBot simulation running on server
 */

#include "build/boxbot.grpc.pb.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;
using boxbot::RPCSim;
using boxbot::ObservationData;
using boxbot::ControlData;
using boxbot::ExperimentDef;
using boxbot::SimParams;

class Sim;

// callback functions
typedef void (*setup_t)(const ExperimentDef *edef, SimParams *sp);
typedef void (*redraw_t)();
typedef void (*observe_t)(ObservationData *odata);

class RPCSimImpl final : public RPCSim::Service {
public:
    RPCSimImpl(Sim *sim, setup_t f1, redraw_t f2, observe_t f3);

    Status init(grpc::ServerContext *context,
                const ExperimentDef *request,
                SimParams *response) override;

    Status step(grpc::ServerContext *context,
                const ControlData *request,
                ObservationData *response) override;
protected:
    Sim *m_sim;
    // callback fns
    setup_t m_fn_setupSim;
    redraw_t m_fn_redraw;
    observe_t m_fn_observe;

};

void run_server();

#endif // SIMSERVER_H
