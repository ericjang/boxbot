#!/bin/env python

'''
RPC interface to run boxbot simulator in C++
'''

import boxbot_pb2
from borg import xterm_cmd
from grpc.beta import implementations
import ipdb as pdb

import numpy as np

import matplotlib.pyplot as plt

_TIMEOUT_SECONDS = 1000
_stub=None

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

	_udim=sp.u_dim

	x=np.fromstring(sp.x.data, dtype=np.uint8)
	x=np.flipud(x.reshape((sp.x.height,sp.x.width,sp.x.channels)))
	plt.imshow(x)
	plt.show()
	x_dim=(sp.x.width, sp.x.height, sp.x.channels)
	return (x, x_dim, sp.u_dim)

def step(u):
	global _stub
	cdata=boxbot_pb2.ControlData()
	cdata.control_data.extend(u)
	#odata=_stub.step(cdata, _TIMEOUT_SECONDS)
	o=_stub.step(cdata,_TIMEOUT_SECONDS)
	assert(len(o.data)==o.width*o.height*o.channels)
	x=np.fromstring(o.data, dtype=np.uint8)
	#np.save('x.npy',x)
	x=x.reshape((o.height,o.width,o.channels))
	x=np.flipud(x)
	#plt.imshow(x)
	#plt.show()
	#return x.flat
	return x