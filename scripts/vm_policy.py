#!/bin/env python

class VisuomotorPolicy(object):
	"""
	visuomotor policy \pi : X -> U that maps single observation
	directly to control
	"""
	def __init__(self, x_dim, u_dim):
		super(VMPolicy, self).__init__()
		self.x_dim=x_dim
		self.u_dim=u_dim
		# build graph here
	
	def getU(self):
		# returns the tensor u
		pass

	def eval(self,xval):
		# actually computes the value
		pass