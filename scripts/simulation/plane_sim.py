
import matplotlib.pyplot as plt

import numpy as np
from numpy.random import randint
import os
import ipdb as pdb


w,h=40,40
rw=1 # robot half-width
p=None
im=None

def is_colliding(p):
  if np.any([p-rw<0, p+rw>=w]):
    return True
  # check robot body overlap with obstacle field
  return np.mean(self.im[p[0]-rw:p[0]+rw+1, p[1]-rw:p[1]+rw+1]) > 0.05

def getX(p):
  # return image X given true state p (position) of robot
  x=np.copy(im)
  x[p[0]-rw:p[0]+rw+1, p[1]-rw:p[1]+rw+1]=1. # robot is white on black background
  return x.reshape((1,h,w,1))

def init(env_file):
  global im
  global p
  im=plt.imread(env_file) # grayscale
  p=[rw,randint(rw,w-rw)] # initial location
  return getX(p) # initial image

def step(u):
  global p
  d=np.sign(u.flat[0])
  nsteps=int(np.abs(u.flat[0]))
  for i in range(nsteps):
    p[0]+=d
    if is_colliding(p):
      p[0]-=d
      break
  d=np.sign(u.flat[1])
  nsteps=int(np.abs(u.flat[1]))
  for i in range(nsteps):
    p[1]+=d
    if is_colliding(p):
      p[1]-=d
      break
  return getX(p)
  


