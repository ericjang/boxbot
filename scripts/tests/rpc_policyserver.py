#!/bin/env python

"""
policy server that uses 

$ cd boxbot
$ python examples/rpc_policyserver.py
"""

import boxbot_pb2
import time
import numpy as np
#import tensorflow as tf

_ONE_DAY_IN_SECONDS = 60 * 60 * 24

class RPCPolicyServicer(boxbot_pb2.BetaRPCPolicyServicer):
  """gRPC policy server to interface with boxbot"""
  
  def __init__(self, u_dim):
    super(RPCPolicyServicer, self).__init__()
    self.u_dim=u_dim

  def GetControls(self, Obsv, context):
    # X is instance of boxbot_pb2.Observation
    cdata = boxbot_pb2.ControlData()
    t=Obsv.time
    #print('GetControls t=', t)
    for i in range(self.u_dim):
      u=0
      if i % 2 == 0:
        u=20*np.sin(t+i)
      cdata.control_data.append(u)
    return cdata

def serve():
  server = boxbot_pb2.beta_create_RPCPolicy_server(RPCPolicyServicer(u_dim=13))
  server.add_insecure_port('[::]:50051')
  server.start()
  print('Running server on port 50051...')
  try:
    while True:
      time.sleep(_ONE_DAY_IN_SECONDS)
  except KeyboardInterrupt:
    server.stop(0)

if __name__=='__main__':
  serve()

