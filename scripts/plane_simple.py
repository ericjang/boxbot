#!/bin/env python

'''
naive E2C with pre-computed database on plane task
'''

import tensorflow as tf
from e2c.e2c_plane_model import E2CPlaneModel
from data.plane_data2 import PlaneData
import ipdb as pdb

batch_size=128
u_dim=2
u=tf.placeholder(tf.float32, [batch_size, u_dim]) # control at time t
e2c = E2CPlaneModel(u, batch_size)
e2c.buildLoss(lambd=.25)

for v in tf.all_variables():
    print("%s : %s" % (v.name, v.get_shape()))
pdb.set_trace()

e2c.buildTrain()

init=tf.initialize_all_variables()
sess=tf.InteractiveSession()
sess.run(init)
# WRITER
writer = tf.train.SummaryWriter("/ltmp/e2c", sess.graph_def)
dataset=PlaneData("data/plane1.npz","data/env1.png")
dataset.initialize()

# resume training
#saver.restore(sess, "/ltmp/e2c-plane-83000.ckpt")
train_iters=2e4 # 5K iters
for i in range(int(train_iters)):
  tup=dataset.sample(batch_size, replace=False)
  loss = e2c.update(sess,tup)
  if i%1000==0:
    print("iter=%d : Loss: %f" % (i,loss))
sess.close()