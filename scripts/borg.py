#!/bin/env python

'''
scripts for sending work across CPUs/GPUs in the CIT.
'''

from subprocess import Popen

def xterm_cmd(cmd):
	# '-title',title,
	cmds=['xterm','-geometry','65x25','-bg','black','-fg','white','-e',cmd]
	Popen(cmds)

def ssh_xterm_cmd(host, cmd):
	cmds=['ssh',host,'xterm','-geometry','65x25','-bg','black','-fg','white','-e',cmd]
	Popen(cmds)
