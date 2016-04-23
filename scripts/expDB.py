#!/bin/env python

'''
module for caching state-action trajectories 
experienced by the robot.
'''



# TODO - use pytables
#http://www.pytables.org/


import os
import sys
from sqlalchemy import Column, ForeignKey, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import relationship
from sqlalchemy import create_engine

Base = declarative_base()

class E2CData(Base):
	__tablename__ = 'e2c'
	# define the columns
	traj_id=Column(Integer)
	t=Column(Integer,primary_key=True)
	x0=
	u0=
	x1=

# Create an engine that stores data in the local directory's
# sqlalchemy_example.db file.
engine = create_engine('sqlite:///sqlalchemy_example.db')

# Create all tables in the engine. This is equivalent to "Create Table"
# statements in raw SQL.
Base.metadata.create_all(engine)

class ExperienceDB(object):
	"""
	caches, deserializes, serializes state-action trajectories
	"""
	def __init__(self, arg):
		super(ExperienceDB, self).__init__()
		self.arg = arg
