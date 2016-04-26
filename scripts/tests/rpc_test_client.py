# TODO - test to make sure client works

# https://github.com/grpc/grpc/blob/master/examples/python/helloworld/greeter_server.py

import boxbot_pb2
from grpc.beta import implementations
import boxbot_pb2



_TIMEOUT_SECONDS = 1

def run():
	channel=implementations.insecure_channel('localhost', 50051)
	stub=boxbot_pb2.beta_create_RPCPolicy_stub(channel)
	x=boxbot_pb2.ObservationData()
	x.time=0
	u=stub.GetControls(x, _TIMEOUT_SECONDS)
	print("received controls from server", u.control_data)

if __name__=='__main__':
	run()