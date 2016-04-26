#!/bin/env python

'''
PyTables-based module for caching state-action trajectories 
experienced by the robot.

use h5py
http://docs.h5py.org/en/latest/quick.html

'''
import h5py
import numpy as np
import ipdb as pdb

class ExpDB(object):
	"""docstring for ExpDB"""
	def __init__(self):
		super(ExpDB, self).__init__()
		
	def init(self, filename, traj_name, x_dim, u_dim, cache_size):
		self.filename=filename
		self.f = h5py.File(filename,'w')
		self.T=50 # trajectory length. truncated upon close
		self.x_dim_flat=np.prod(x_dim)
		#self.t1 = self.f.create_dataset(traj_name + '/t1', (self.T, 1), dtype='i', maxshape=(None,1))
		self.u0 = self.f.create_dataset(traj_name + '/u0', (self.T, u_dim), dtype='f', maxshape=(None,u_dim))
		self.x1 = self.f.create_dataset(traj_name + '/x1', (self.T, self.x_dim_flat), dtype=np.uint8, maxshape=(None,self.x_dim_flat))
		self.traj_name=traj_name
		self.cache_size=cache_size

		self.u_dim=u_dim
		self.x_dim=x_dim
		
		self.t = 0 # dataset head - where to insert next
		self.i = 0 # cache head - where to insert next into cache
		self.reset_cache()

	def close(self):
		self.new_cycle() # flush cache
		# resize dataset to fit trajectory we have so far
		self.u0.resize((self.t,self.u_dim))
		self.x1.resize((self.t,self.x_dim_flat))
		self.f.close()
		print('%s saved to disk' % (self.filename))

	def load(self, filename, cache_size, x_dim):
		# loads state from existing file
		self.filename=filename
		self.f = h5py.File(filename,'r+')
		# assume trajectory is the first group for now
		self.traj_name=self.f.keys()[0]
		self.u0=self.f[self.traj_name + '/u0']
		self.x1=self.f[self.traj_name + '/x1']
		self.T =self.u0.len()
		self.u_dim=self.u0.shape[1]
		self.x_dim_flat=self.x1.shape[1]
		assert(np.prod(x_dim)==self.x_dim_flat)
		self.x_dim=x_dim

		self.t = self.T
		self.cache_size=cache_size
		self.reset_cache()
		print('%s loaded from disk' % (self.filename))

	def reset_cache(self):
		#self.t1_cache=np.zeros((self.cache_size,1),dtype=np.int)
		self.u0_cache=np.zeros((self.cache_size,self.u_dim),dtype=np.float32)
		self.x1_cache=np.zeros((self.cache_size,self.x_dim_flat),dtype=np.uint8)
		self.i=0
	
	def new_cycle(self):
		# flushes day trajectory to persistent DB
		# run out of space, double capacity
		t=self.t
		n=self.i

		if t + n > self.T:
			print('resizing to capacity=', self.T*2)
			self.T *= 2
			#self.t1.resize((self.T,1))
			self.u0.resize((self.T,self.u_dim))
			self.x1.resize((self.T,self.x_dim_flat))

		#self.t1[t:t+cs,:] = self.t1_cache
		self.u0[t:t+n,:] = self.u0_cache[:n,:]
		self.x1[t:t+n,:] = self.x1_cache[:n,:]

		self.f.flush() # flush buffers to disk

		self.t += n
		self.reset_cache()

	def append(self, u, x):
		assert(self.i < self.cache_size)
		#self.t1_cache[self.i,:]=t
		self.u0_cache[self.i,:]=u
		self.x1_cache[self.i,:]=x.flat
		self.i+=1

	def sample(self, batch_size, k):
		X0=np.zeros((batch_size,self.x_dim_flat))
		U0=np.zeros((batch_size,self.u_dim))
		X1=np.zeros((batch_size,self.x_dim_flat))
		
		num_day = int(np.ceil(k * batch_size))
		num_exp = batch_size-num_day
		if num_day > 0:
			i=np.random.choice(self.cache_size-1,size=num_day,replace=False)
			X0[:num_day,:]=self.x1_cache[i-1,:]
			U0[:num_day,:]=self.u0_cache[i,:]
			X1[:num_day,:]=self.x1_cache[i,:]

		if num_exp > 0:
			# choose from experience replay
			# fancy indexing must be provided in increasing order
			j=np.sort(np.random.choice(self.t-1,size=num_exp,replace=False))	
			uset=self.f[self.traj_name + '/u0']
			xset=self.f[self.traj_name + '/x1']
			X0[num_day:,:]=xset[j-1,:]
			U0[num_day:,:]=uset[j,:]
			X1[num_day:,:]=xset[j,:]

		# reshape X0, X1
		X0=X0.reshape((-1,)+self.x_dim).astype(np.float32)/255
		X1=X1.reshape((-1,)+self.x_dim).astype(np.float32)/255

		return (X0,U0,X1)
		


		
		