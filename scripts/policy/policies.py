#!/bin/env python

# https://github.com/tensorflow/tensorflow/blob/r0.8/tensorflow/models/image/cifar10/cifar10.py

import tensorflow as tf
import numpy as np
from abc import ABCMeta, abstractmethod
from e2c.nn import *
import ipdb as pdb

class Policy(object):
  __metaclass__ = ABCMeta
  """abstract policy \pi : X -> U that maps single observation
  directly to control"""
  def __init__(self, batch_size, x_dim, u_dim):
    super(Policy, self).__init__()
    self.x_dim=x_dim
    self.u_dim=u_dim
    self.batch_size=batch_size
  
  @abstractmethod
  def eval(self, x):
    return NotImplemented

  def getOutput(self):
    return tf.placeholder(tf.float32, (self.batch_size,)+self.u_dim)

class RandomPolicy(Policy):
  '''
  random policy - for benchmarking E2C model 
  on a 'static' dataset
  '''
  def __init__(self, batch_size, x_dim, u_dim):
    super(RandomPolicy, self).__init__(batch_size, x_dim, u_dim)

  def eval(self, x):
    return np.random.randn(self.u_dim)

class VisuomotorPolicy(Policy):
  """docstring for VisualMotorPolicy"""
  def __init__(self,batch_size,x_dim,u_dim,share=None):
    super(VisuomotorPolicy, self).__init__(batch_size,x_dim,u_dim)
    self.x = tf.placeholder(tf.float32, (self.batch_size,)+self.x_dim)
    self.buildModel(share)

  def buildModel(self,share):
    with tf.variable_scope('conv1',reuse=share) as scope: # inherits share
      # kernel shape is filter_height, filter_width, in_channels, out_channels
      #kernel=var_with_decay('weights',shape=[4,3,3,64],stddev=1e-4,wd=0.)
      c=self.x.get_shape()[3].value
      kernel=tf.get_variable('weights', [3,4,c,16], initializer=tf.truncated_normal_initializer(stddev=1e-4))
      conv=tf.nn.conv2d(self.x,kernel,[1,1,1,1],padding='SAME')
      biases=tf.get_variable('biases',[16],initializer=tf.constant_initializer(0.))
      bias=tf.nn.bias_add(conv,biases)
      conv1=tf.nn.relu(bias,name=scope.name)
      activation_summary(conv1)
      # pool1
      pool1=tf.nn.max_pool(conv1,ksize=[1,3,3,1],strides=[1,2,2,1],padding='SAME',name='pool1')
      # norm1
      norm1 = tf.nn.lrn(pool1, 4, bias=1.0, alpha=0.001 / 9.0, beta=0.75,name='norm1')
      x_flat_dim=np.prod(self.x_dim)
      x=tf.reshape(norm1,[self.batch_size,-1]) # flatten
      for l in range(2):
        x=ReLU(x,150,"l"+str(l))
      self.u= ReLU(x,self.u_dim,"u")

  def eval(self,x):
    return self.sess.run(self.u,{self.x:x})

  def set_session(self,sess):
    self.sess=sess
    
  def set_reward(self,r):
    # set objectie to minimize tensor -R
    self.reward = r
    self.buildTrain()

  def buildTrain(self):
    # TODO - need to only optimize over specific variables
    with tf.variable_scope("Optimizer"):
      learning_rate=1e-4
      optimizer=tf.train.AdamOptimizer(learning_rate, beta1=0.1, beta2=0.1) # beta2=0.1
      self.train_op=optimizer.minimize(-self.reward)

  def buildSummaries(self):
    tf.scalar_summary("R", tf.reduce_mean(L_x))
    self.all_summaries = tf.merge_all_summaries()

  def update(self,tup):
    x0, u0, x1 = tup
    feed_dict={self.x:x0,self.u:u0,self.x_next:x1}
    results=self.sess.run([self.reward,self.train_op,self.all_summaries],feed_dict)
    return results

Policy.register(RandomPolicy)
Policy.register(VisuomotorPolicy)

