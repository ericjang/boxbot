#ifndef RPCPOLICY_H
#define RPCPOLICY_H


#include "policy.h"
#include <grpc++/grpc++.h>
#include "build/boxbot.pb.h"
#include "build/boxbot.grpc.pb.h"
#include <memory>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class RPCPolicy : public Policy
{
public:
    RPCPolicy(uint xdim, uint udim, std::shared_ptr<Channel> channel);
    const ControlData computeControls(const ObservationData &data);
protected:
    std::unique_ptr<boxbot::RPCPolicy::Stub> m_stub;
};

#endif // RPCPOLICY_H
