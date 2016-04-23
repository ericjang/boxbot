#!/bin/env python


import boxbot_pb2

E=boxbot_pb2.ExperimentDef()
E.robot = "polyp"

num_cycles=1000 # total overall cycles

A=100

B=100
k=.2 # 20% of minibatch sampled from previous 'day phase'

C=100

e2c_batch_size=128
policy_batch_size=128

