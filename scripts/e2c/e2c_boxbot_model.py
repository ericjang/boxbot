#!/bin/env python

'''
this is currently a hot mess, need to make better encode/decode fns
'''

from e2c_model import E2CModel
import tensorflow as tf
from nn import *
import ipdb as pdb

class E2CBoxbotModel(E2CModel):
  def __init__(self, x,u,z_dim):
    super(E2CBoxbotModel, self).__init__(x, u, z_dim)
    
  def encode(self,x,share=None):

    with tf.variable_scope("encoder",reuse=share):
      # conv1 layer
      with tf.variable_scope('conv1') as scope: # inherits share
        # kernel shape is filter_height, filter_width, in_channels, out_channels
        #kernel=var_with_decay('weights',shape=[4,3,3,64],stddev=1e-4,wd=0.)
        kernel=tf.get_variable('weights', [3,4,3,16], initializer=tf.truncated_normal_initializer(stddev=1e-4))
        conv=tf.nn.conv2d(x,kernel,[1,1,1,1],padding='SAME')
        biases=tf.get_variable('biases',[16],initializer=tf.constant_initializer(0.))
        bias=tf.nn.bias_add(conv,biases)
        conv1=tf.nn.relu(bias,name=scope.name)
        activation_summary(conv1)

      # pool1
      pool1=tf.nn.max_pool(conv1,ksize=[1,3,3,1],strides=[1,2,2,1],padding='SAME',name='pool1')
      # norm1
      norm1 = tf.nn.lrn(pool1, 4, bias=1.0, alpha=0.001 / 9.0, beta=0.75,
                    name='norm1')
      
      # conv2
      with tf.variable_scope('conv2') as scope:
        #kernel = var_with_decay('weights', shape=[5, 5, 64, 64],
        #                                     stddev=1e-4, wd=0.0)
        kernel=tf.get_variable('weights',[3,4,16,16],initializer=tf.truncated_normal_initializer(stddev=1e-4))
        conv = tf.nn.conv2d(norm1, kernel, [1, 1, 1, 1], padding='SAME')
        biases = tf.get_variable('biases', [16], initializer=tf.constant_initializer(0.1))
        bias = tf.nn.bias_add(conv, biases)
        conv2 = tf.nn.relu(bias, name=scope.name)
        activation_summary(conv2)

      # norm2
      norm2 = tf.nn.lrn(conv2, 4, bias=1.0, alpha=0.001 / 9.0, beta=0.75,
                    name='norm2')
      # pool2
      pool2 = tf.nn.max_pool(norm2, ksize=[1, 3, 3, 1],
                         strides=[1, 2, 2, 1], padding='SAME', name='pool2')
      # reshape to do local
      x=tf.reshape(pool2,[self.batch_size,-1]) # flatten
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
      # up-convolution
      with tf.variable_scope('deconv1') as scope:
        h,w,out_c=self.x_dim
        in_channels=200
        z=tf.reshape(z,(-1,1,1,in_channels))
        kernel=tf.get_variable('weights',[3,4,out_c,in_channels],initializer=tf.truncated_normal_initializer(stddev=1e-4))
        output_size=[self.batch_size,h,w,out_c]
        deconv1=tf.nn.deconv2d(z,kernel,output_size,[1,1,1,1],padding='SAME')
        return deconv1

  def sampleP_theta(self,h_dec,share=None):
    # override to avoid linear
    with tf.variable_scope("P_theta",reuse=share):
      x= tf.sigmoid(h_dec) # mean of bernoulli distribution
      return x