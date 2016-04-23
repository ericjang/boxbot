#!/bin/env python

"""
Generic E2C model. Subclass this to implement custom submodules.
"""

class E2CModel(object):
	def __init__(self, arg):
		super(E2CModel, self).__init__()
		self.arg = arg
