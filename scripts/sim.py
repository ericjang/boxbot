#!/bin/env python

'''
RPC interface to run boxbot simulator in C++
'''

import boxbot_pb2
from borg import xterm_cmd
from grpc.beta import implementations
import ipdb as pdb

_TIMEOUT_SECONDS = 1
_stub=None

_xdim=None
_udim=None

def init(E,host="localhost",port=50051):
	'''
	initializes C++ process, then 
	sends RPC request to C++ server to initialize the simulation
	we get back x_dim, u_dim
	'''
	#xterm_cmd("/home/evjang/boxbot/boxbot server")
	# connect to it
	global _stub
	channel=implementations.insecure_channel(host,port)
	_stub=boxbot_pb2.beta_create_RPCSim_stub(channel)
	sp=_stub.init(E, _TIMEOUT_SECONDS)

	_xdim=sp.x_dim
	_udim=sp.u_dim

	return (sp.x_dim, sp.u_dim)

def step(u):
	global _stub
	cdata=boxbot_pb2.ControlData()
	cdata.control_data.extend(u)
	#odata=_stub.step(cdata, _TIMEOUT_SECONDS)
	odata=_stub.step(cdata,1000)
	x = odata.data
	return None
	# TODO - extract observation numpy array
	# and return it
