#!/usr/bin/env python

"""Statistics library for anomaly detection."""
__author__ = "Julien Beaudaux"
__copyright__ = "Copyright 2014, The Blue print Project"
__version__ = "0.1"
__email__ = "julienbeaudaux@gmail.com"

# Copyright (C) 2014 by Julien Beaudaux for the Blue print project
# 
#                        All Rights Reserved
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose is forbidden unless specific
# prior written permission is granted by its original owners;
#
# Neither the name of the Blue print project nor the names of its
# contributors may be used to endorse or promote products derived
# from this software without specific prior written permission.
#
# The authors and the Blue print project disclaims all warranties
# with regard to this software, including all implied warranties
# of merchantability and fitness. In no wvent shall the authors and
# the Blue print project be liable for any special, indirect or
# consequencial damages or any damages whatsoever.

import math


# Calculate iteratively the sum of squared deviations from the mean
def updateSSE(li):
	t = 0
	moy = 0
	SSE = 0
	
	for xt in li:
		t += 1
		et = xt - moy
		#print xt, moy, et
		moy = moy + (et/t)
		SSE = SSE + (et *(xt - moy))
		#print xt, moy, SSE
	
	# We then get the SSE, the 
	return moy, SSE, SSE/(t-1), math.sqrt(SSE/(t-1))


# Calculates statistical parameters of a sample
def stats(vals, confinter=95):
	tot = sum(vals)
	nbv = len(vals)
	avg = tot / float(nbv)
	medi = vals[nbv/2]
	maxi = max(vals)
	mini = min(vals)
	
	vari = sum([(i-avg)**2 for i in vals])/(nbv-1 or 1)
	stdv = math.sqrt(vari)
	
	conf_map = {90: 1.65, 95: 1.96, 99: 2.58}
	if confinter not in conf_map:
		print "Warning: Invalid confidence level (should be 90, 95 or 99)."
	else:
		marginError = conf_map[confinter] * (float(stdv)/(math.sqrt(float(nbv))))
	
	return avg, medi, vari, stdv, mini, maxi, marginError


# Just a test
#li = [2.,5.,8.,1.,0.,0.,9.,3.,2.,8.,6.,4.,5.]
#print stats(li)
#print updateSSE(li)