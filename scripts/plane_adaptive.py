#!/bin/env python

'''
Runs joint training of exploration policy and E2C learning for the plane task
'''
from e2c.e2c_plane_model import E2CPlaneModel
#from data.plane_data2 import PlaneData
from policy.policies import VisuomotorPolicy
from simulation import plane_sim as sim
import ipdb as pdb
import numpy as np
from data.expDB import ExpDB
from analysis import viz
import tensorflow as tf

num_cycles=10 # total overall cycles
A=129 # number of observations in a day cache
B=100 # number of E2C minibatches to learn from per cycle
k=.2 # 20% of minibatch sampled from previous 'day phase'
C=100 # number of explore policy updates to learn from per cycle
batch_size=128
filename="data/plane.h5"
x_dim=(40,40,1)
u_dim=2

expDB=ExpDB()
expDB.init(filename, "traj0", x_dim, u_dim, A)

# E2C training & Policy training are run/trained in this process for now
policy_eval=VisuomotorPolicy(1, x_dim, u_dim)
policy_train=VisuomotorPolicy(batch_size, x_dim, u_dim, True) # share parameters

# set up computational graph
e2c = E2CPlaneModel(policy_train.u, batch_size)
e2c.buildLoss(lambd=.25)
for v in tf.all_variables():
    print("%s : %s" % (v.name, v.get_shape()))
e2c.buildTrain()
e2c.buildSummaries()

policy_train.set_reward(-e2c.loss)

init=tf.initialize_all_variables()
sess=tf.InteractiveSession()
sess.run(init)
writer = tf.train.SummaryWriter("/ltmp/e2c", sess.graph_def)

policy_eval.set_session(sess)
policy_train.set_session(sess)

x=sim.init("data/env1.png")
x=x.reshape((1,40,40,1))
#viz.compare(x[0,:,:,0],x[0,:,:,0])

t=0 # quick plot hack
for c in range(num_cycles):
  expDB.new_cycle() # flushes day trajectory to persistent DB

  # 'Day' phase
  for i in range(A):
    u=policy_eval.eval(x) # run Explore Policy
    print(u)
    x_next=sim.step(u)
    #viz.compare(x[0,:,:,0],x_next[0,:,:,0])
    expDB.append(u,x_next)
    x=x_next.reshape((1,40,40,1))

  if c==0:
    k=1.

  # 'Night' phase part 1 - update E2C model
  for i in range(B):
    tup=expDB.sample(batch_size,k)
    results = e2c.update(sess,tup,True)
    writer.add_summary(results[2], t)
    t+=1

  # 'Night' phase part 2 - update Explore Policy
  for i in range(C):
    tup=expDB.sample(batch_size,k)
    policy.update(sess,tup)
    writer.add_summary(results[2], t)
    t+=1


expDB.close()
print('done!')