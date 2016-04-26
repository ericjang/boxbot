#!/bin/env python

from e2c_model import E2CModel
#import numpy as np
import tensorflow as tf
#import matplotlib.pyplot as plt
from nn import *
import ipdb as pdb

class E2CPlaneModel(E2CModel):
  def __init__(self, u, batch_size):
    x_dim=1600
    z_dim=2
    x=tf.placeholder(tf.float32,[batch_size, x_dim])
    #u=tf.placeholder(tf.float32, [batch_size, u_dim]) # control at time t
    super(E2CPlaneModel, self).__init__(x, u, z_dim)

  def encode(self,x,share=None):
    with tf.variable_scope("encoder",reuse=share):
      for l in range(3):
        x=ReLU(x,150,"l"+str(l))
      return linear(x,2*self.z_dim)

  def dynamics(self, z):
    with tf.variable_scope("dynamics"):
      for l in range(2):
        z=ReLU(z,100,"l"+str(l))
      return z

  def decode(self, z, share=None):
    with tf.variable_scope("decoder",reuse=share):
      for l in range(2):
        z=ReLU(z,200,"l"+str(l))
      return linear(z,self.x_dim_flat)

  def update(self, sess, tup, write_summary=False): # tmp hack
    x0, u0, x1 = tup
    x0=x0.reshape([self.batch_size,-1])
    x1=x1.reshape([self.batch_size,-1])
    feed_dict={self.x:x0,self.u:u0,self.x_next:x1}
    if write_summary:
      pdb.set_trace()
      results=sess.run([self.loss,self.train_op,self.all_summaries],feed_dict)
    else:
      results=sess.run([self.loss,self.train_op],feed_dict)
    return results