#!/bin/env python

# helpful functions for VAE-type routines

import tensorflow as tf


eps=1e-9 # numerical stability

class NormalDistribution(object):
  """
  Represents a multivariate normal distribution parameterized by
  N(mu,Cov). If cov. matrix is diagonal, Cov=(sigma).^2. Otherwise,
  Cov=A*(sigma).^2*A', where A = (I+v*r^T).
  """
  def __init__(self,mu,sigma,logsigma,v=None,r=None):
    self.mu=mu
    self.sigma=sigma # either stdev diagonal itself, or stdev diagonal from decomposition
    self.logsigma=logsigma
    dim=mu.get_shape()
    if v is None:
      v=tf.constant(0.,shape=dim)
    if r is None:
      r=tf.constant(0.,shape=dim)
    self.v=v
    self.r=r

def KLGaussian(Q,N):
  # Q, N are instances of NormalDistribution
  # implements KL Divergence term KL(N0,N1) derived in Appendix A.1
  # Q ~ Normal(mu,A*sigma*A^T), N ~ Normal(mu,sigma_1)
  # returns scalar divergence, measured in nats (information units under log rather than log2), shape= batch x 1
  sum=lambda x: tf.reduce_sum(x,1) # convenience fn for summing over features (columns)
  k=float(Q.mu.get_shape()[1].value) # dimension of distribution
  mu0,v,r,mu1=Q.mu,Q.v,Q.r,N.mu
  s02,s12=tf.square(Q.sigma),tf.square(N.sigma)+eps
  #vr=sum(v*r)
  a=sum(s02*(1.+2.*v*r)/s12) + sum(tf.square(v)/s12)*sum(tf.square(r)*s02) # trace term
  b=sum(tf.square(mu1-mu0)/s12) # difference-of-means term
  c=2.*(sum(N.logsigma-Q.logsigma) - tf.log(1.+sum(v*r))) # ratio-of-determinants term. 
  return 0.5*(a+b-k+c)#, a, b, c

def binary_crossentropy(t,o):
    return t*tf.log(o+eps) + (1.0-t)*tf.log(1.0-o+eps)

def recons_loss(x,x_recons):
  with tf.variable_scope("Lx"):
    # reshape x, x_recons to flat vectors
    batch_size=x.get_shape()[0].value
    x=tf.reshape(x,(batch_size,-1))
    x_recons=tf.reshape(x,(batch_size,-1))
    ll=tf.reduce_sum(binary_crossentropy(x,x_recons),1) # sum across features
    return -ll # negative log-likelihood

def latent_loss(Q):
  with tf.variable_scope("Lz"):
    mu2=tf.square(Q.mu)
    sigma2=tf.square(Q.sigma)
    # negative of the upper bound of posterior
    return -0.5*tf.reduce_sum(1+2*Q.logsigma-mu2-sigma2,1)

def sampleNormal(mu,sigma):
  # diagonal stdev
  n01=tf.random_normal(sigma.get_shape(), mean=0, stddev=1)
  return mu+sigma*n01
