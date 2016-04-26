#!/bin/env python

'''
control experiment - learn E2C on worm robot
without adaptive exploration policy 
'''

from e2c.e2c_boxbot_model import E2CBoxbotModel
from policy.policies import RandomPolicy
import numpy as np
from data.expDB import ExpDB
from simulation import boxbot_pb2, sim
import time
import tensorflow as tf
import ipdb as pdb
from analysis import viz

# define experiment
# some of these params are useless..
E=boxbot_pb2.ExperimentDef()
E.robot = "polyp"
num_cycles=10 # total overall cycles
A=100 # number of day-cycles
B=100 # number of E2C minibatches
k=.2 # 20% of minibatch sampled from previous 'day phase'
C=100 # number of explore policy updates
e2c_batch_size=128
policy_batch_size=128

def explore(filename):
  (x, x_dim, u_dim) = sim.init(E, host="localhost", port=50051) # start C++ sim
  print(x_dim,u_dim)
  expDB=ExpDB()
  expDB.init(filename, "traj0", x_dim, u_dim, A)
  policy=RandomPolicy(1, x_dim, u_dim)
  for i in range(num_cycles):
    expDB.new_cycle()
    for j in range(A):
      #time.sleep(1./60) # oh god this hack is terrible (avoid deadlock with my bad C++ code)
      u=policy.eval(x) # run Explore Policy
      x_next=sim.step(u)
      expDB.append(u,x_next)
      x=x_next
    print(i)
  expDB.close()

def trainE2C(filename):
  batch_size=64
  x_dim=(240, 320, 3) # height, width, channels
  u_dim=(13,)
  z_dim=2

  expDB=ExpDB()
  expDB.load(filename, 0, x_dim)
  
  policy=RandomPolicy(batch_size,x_dim,u_dim)

  x=tf.placeholder(tf.float32,(batch_size,)+x_dim)
  u=policy.getOutput()
  e2c=E2CBoxbotModel(x, u, z_dim)
  e2c.buildLoss(lambd=.25)
  
  # check variables
  for v in tf.all_variables():
    print("%s : %s" % (v.name, v.get_shape()))
  #pdb.set_trace()

  e2c.buildTrain()
  e2c.buildSummaries()

  # train
  init=tf.initialize_all_variables()
  sess=tf.InteractiveSession()
  sess.run(init)
  writer = tf.train.SummaryWriter("/ltmp/e2c", sess.graph_def)
  train_iters=2e3 # 5K iters
  for i in range(int(train_iters)):
    tup=expDB.sample(batch_size,k=0)
    # visualize
    #viz.compare(tup[0][0,:,:,:], tup[2][0,:,:,:])
    results = e2c.update(sess,tup,True)
    print(results[0])
    writer.add_summary(results[2], i)

  sess.close()

if __name__ == '__main__':
  filename="data/worm.h5"
  #explore(filename)
  trainE2C(filename)
  print('done!')