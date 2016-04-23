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

class RPCSimImpl final : public RPCSim::Service {
public:
    RPCSimImpl(Sim *sim, void (*f1)(const ExperimentDef *edef), void (*f2)());

    Status init(grpc::ServerContext *context,
                const ExperimentDef *request,
                SimParams *response) override;

    Status step(grpc::ServerContext *context,
                const ControlData *request,
                ObservationData *response) override;
protected:
    Sim *m_sim;
    // callback fns
    void (*m_fn_initSim)(const ExperimentDef *edef);
    void (*m_fn_redraw)();
    // todo
    void (*m_observe)();
};

void run_server();

#endif // SIMSERVER_H
