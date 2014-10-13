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

# Calculate variance of a sample
def variance(l):
	print "lol"

# Calculates statistical parameters of a sample
def stats(vals, confidence_interval=0.05):
	tot = sum(vals)
	nbv = len(vals)
	avg = tot / float(nbv)
	sortVals = vals.sort()
	medi = sortVals[nbv/2]
	maxi = vals.max()
	mini = vals.min()
	vari = variance(vals)
	stdv = math.sqrt(vari)
	
	return avg, medi, vari, stdv, mini, maxi
