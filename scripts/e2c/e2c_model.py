#!/bin/env python

"""
Generic E2C model. Subclass this to implement custom submodules.
"""
from abc import ABCMeta, abstractmethod
import tensorflow as tf
import numpy as np
from nn import *
from variational import *
import ipdb as pdbL_

class E2CModel(object):
  __metaclass__ = ABCMeta
  def __init__(self, x, u, z_dim):
    # x = [batch_size, d0, d1, ...]
    # u = [batch_size, u_dim]
    super(E2CModel, self).__init__()
  
    self.batch_size=x.get_shape()[0].value #regrettable hack, but needed for sampleNormal to work
    self.x=x # dim not necessarily flattened
    self.u=u

    self.x_dim=tuple(x.get_shape().as_list()[1:])
    self.u_dim=u.get_shape().as_list()[1]
    self.x_dim_flat=np.prod(self.x_dim)
    self.z_dim=z_dim

    self.buildModel()
    
  @abstractmethod
  def encode(self, x, share=None):
    # X -> H_ENC
    # it is up to the encoder to deal with x_dim not
    # being flat
    return NotImplemented

  def sampleQ_phi(self, h_enc,share=None):
    # H_ENC -> Z
    with tf.variable_scope("sampleQ_phi",reuse=share):
      mu,log_sigma=tf.split(1,2,linear(h_enc,self.z_dim*2)) # diagonal stdev values
      sigma=tf.exp(log_sigma)
      return sampleNormal(mu,sigma), NormalDistribution(mu, sigma, log_sigma)

  @abstractmethod
  def dynamics(self, z):
    # map Z -> H_TRANS
    return NotImplemented

  def transition(self, z):
    # compute A,B,o linearization matrices
    # Z -> H_TRANS -> (A,B,o)
    h=self.dynamics(z)
    z_dim=self.z_dim
    u_dim=self.u_dim
    with tf.variable_scope("trans"):
      with tf.variable_scope("A"):
        v,r=tf.split(1,2,linear(h,z_dim*2))
        v1=tf.expand_dims(v,-1) # (batch, z_dim, 1)
        rT=tf.expand_dims(r,1) # batch, 1, z_dim
        I=tf.diag([1.]*z_dim)
        A=(I+tf.batch_matmul(v1,rT)) # (z_dim, z_dim) + (batch, z_dim, 1)*(batch, 1, z_dim) (I is broadcasted) 
      with tf.variable_scope("B"):
        B=linear(h,z_dim*u_dim)
        B=tf.reshape(B,[-1,z_dim,u_dim])
      with tf.variable_scope("o"):
        o=linear(h,z_dim)
      return A,B,o,v,r

  def sampleQ_psi(self,z,u,Q_phi):
    # Z->Z
    A,B,o,v,r=self.transition(z)
    with tf.variable_scope("sampleQ_psi"):
      mu_t=tf.expand_dims(Q_phi.mu,-1) # batch,z_dim,1
      Amu=tf.squeeze(tf.batch_matmul(A,mu_t), [-1])
      u=tf.expand_dims(u,-1) # batch,u_dim,1
      Bu=tf.squeeze(tf.batch_matmul(B,u),[-1])
      Q_psi=NormalDistribution(Amu+Bu+o,Q_phi.sigma,Q_phi.logsigma, v, r)
      # the actual z_next sample is generated by deterministically transforming z_t
      z=tf.expand_dims(z,-1)
      Az=tf.squeeze(tf.batch_matmul(A,z),[-1])
      z_next=Az+Bu+o
      return z_next,Q_psi#,(A,B,o,v,r) # debugging

  @abstractmethod
  def decode(self, z, share=None):
    # Z -> H_DEC
    return NotImplemented

  def sampleP_theta(self,h_dec,share=None):
    # H_DEC->X 
    with tf.variable_scope("P_theta",reuse=share):
      p=linear(h_dec,self.x_dim_flat)
      x= tf.sigmoid(p) # mean of bernoulli distribution
      x=tf.reshape(x,(-1,)+self.x_dim) # reshape to original image dimensions
      return x

  def buildModel(self):
    x_next=tf.placeholder(tf.float32, (self.batch_size,)+self.x_dim)

    # encode x_t
    h_enc=self.encode(self.x)
    z,Q_phi=self.sampleQ_phi(h_enc)
    # reconstitute x_t
    h_dec=self.decode(z)
    x_recons=self.sampleP_theta(h_dec)
    # compute linearized dynamics, predict new latent state
    z_predict,Q_psi=self.sampleQ_psi(z,self.u,Q_phi)
    # decode prediction
    h_dec_predict=self.decode(z_predict,share=True)
    x_predict=self.sampleP_theta(h_dec_predict,share=True)
    # encode next 
    h_enc_next=self.encode(x_next,share=True)
    z_next,Q_phi_next=self.sampleQ_phi(h_enc_next,share=True)

    # save variables
    self.x_next=x_next
    self.x_recons=x_recons
    self.x_predict=x_predict
    self.Q_phi=Q_phi
    self.Q_psi=Q_psi
    self.Q_phi_next=Q_phi_next
  
  def buildLoss(self, lambd):
    # code may need to be modified to adjust for this
    with tf.variable_scope("Loss"):
      L_x=recons_loss(self.x,self.x_recons)
      L_x_next=recons_loss(self.x_next,self.x_predict)
      L_z=latent_loss(self.Q_phi)
      L_bound=L_x+L_x_next+L_z
      KL=KLGaussian(self.Q_psi,self.Q_phi_next)
      self.loss=tf.reduce_mean(L_bound+lambd*KL) # average loss over minibatch to single scalar
      # loss summaries
      tf.scalar_summary("L_x", tf.reduce_mean(L_x))
      tf.scalar_summary("L_x_next", tf.reduce_mean(L_x_next))
      tf.scalar_summary("L_z", tf.reduce_mean(L_z))
      tf.scalar_summary("KL",tf.reduce_mean(KL))
      tf.scalar_summary("loss", self.loss)

  def buildTrain(self):
    with tf.variable_scope("Optimizer"):
      learning_rate=1e-4
      optimizer=tf.train.AdamOptimizer(learning_rate, beta1=0.1, beta2=0.1) # beta2=0.1
      self.train_op=optimizer.minimize(self.loss)

  def buildSummaries(self):
    self.all_summaries = tf.merge_all_summaries()

  def update(self, sess, tup, write_summary=False):
    x0, u0, x1 = tup
    # these should NOT be flattened
    feed_dict={self.x:x0,self.u:u0,self.x_next:x1}
    if write_summary:
      results=sess.run([self.loss,self.train_op,self.all_summaries],feed_dict)
    else:
      results=sess.run([self.loss,self.train_op],feed_dict)
    return results


  



