#!/bin/env python

'''
Runs joint training of exploration policy and E2C learning
'''

from e2c_model import E2CModel
from vm_policy import VisuomotorPolicy
import sim
import params as p
import ipdb as pdb
import numpy as np
from expDB import ExpDB

#x_dim is a tuple (height,width,channels)
(x_dim, u_dim) = sim.init(p.E,host="localhost",port=50051) # start C++ sim


expDB=ExpDB("boxbot_worm.h5", "0", x_dim, u_dim, p.A)

import time
# step the simulation forward a couple times
for t in range(2000):
	#time.sleep(1./60)
	u=np.random.randn(u_dim)
	x_next=sim.step(u)
	expDB.append(t, u, x_next.flat)

# tmp

import sys
sys.exit()

# E2C training & Policy training are run/trained in this process for now
policy=VisuomotorPolicy(x_dim, u_dim)

# set up computational graph
e2c=E2CModel(x_dim, policy.getU())
policy.set_reward(e2c.loss)

for c in range(p.num_cycles):
	expDB.new_cycle() # flushes day trajectory to persistent DB

	# 'Day' phase
	for i in range(p.A):
		u=policy.eval(x) # run Explore Policy
		x_next=sim.step(u)
		expDB.append(u,x_next)
		x=x_next

	# 'Night' phase part 1 - update E2C model
	for i in range(p.B):
		(x0,u0,x1)=expDB.sample(p.e2c_batch_size,recent=k)
		feed_dict={ e2c.x0 : x0, e2c.u0 : u0,	e2c.x1 : x1 }
		e2c.update(feed_dict)

	# 'Night' phase part 2 - update Explore Policy
	for i in range(p.C):
		(x0,u0,x1)=expDB.sample(p.policy_batch_size,recent=k)
		feed_dict={ e2c.x0 : x0, e2c.u0 : u0,	e2c.x1 : x1 }
		policy.update(feed_dict)

print('done!')