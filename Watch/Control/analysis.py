#!/usr/bin/env python

"""Analyses the data from demo mode to detect anomalies."""
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
import statistics


class anomalyDetection():
	
	#iteration = 0	# Number of iterations from the first sample
	#moyVal = 0
	#moySSE = 0
	#SSE = 0
	
	
	def __init__(self):
        	self.iteration = 0
        	self.moyVal = 0
        	self.moySSE = 0
        	self.SSE = 0
        	self.stdv = 0
        	
        	self.threshF = 4.
        	self.threshL = 0.5
        	
        	self.aggregation = 50	# Aggregate 1s of data altogether (50)
        	self.dataAgg = []
	
	
	# Makes calculations of SSE and stdv on the run
	def iterateSSE(self, norm):
		self.moyVal += norm
		self.iteration += 1
		et = float(norm) - self.moySSE
		self.moySSE = self.moySSE + (et/self.iteration)
		self.SSE = self.SSE + (et *(float(norm) - self.moySSE))
		
		# Finds current standard deviation
		ret = 0 if self.iteration < 2 else math.sqrt(self.SSE/(self.iteration-1))
		self.stdv = ret
		#self.labelVariable.set("STDV = %f \t VAL = %f"%(ret, norm))
		#print "STDV = %f \t VAL = %f"%(ret, norm)
		return ret
	
	
	def aggregateActivity(self, value):
		diffs = 0.
		if len(self.dataAgg) >= self.aggregation:
			i = 0
			while i < len(self.dataAgg)-1:
				diffs += self.dataAgg[i+1] - self.dataAgg[i] if self.dataAgg[i] < self.dataAgg[i+1] else self.dataAgg[i] - self.dataAgg[i+1]
				i += 1
			diffs /= i
			
			self.iterateSSE(diffs)
			#self.categorizeActivity(diffs)
			self.dataAgg = []
		else:
			self.dataAgg.append(value)
		return diffs
	
	
	# Detects walking, falling and lying patterns from data
	def categorizeActivity(self, diffs):
		#print "Categ", self.dataAgg, diffs, self.stdv
		# Compares movement to stdv to discriminate patterns
		if diffs >= self.threshF * self.stdv:
			print "Falling !!! (%f // %f)"%(diffs, self.stdv)
		elif diffs <= self.threshL * self.stdv:
			print "Lying ... (%f // %f)"%(diffs, self.stdv)
		else:
			print "Normal activity ... (%f // %f)"%(diffs, self.stdv)
	
	
	# Uses provided data to discriminate activity patterns
	def detectFall(self, accel):
		return self.aggregateActivity(accel), self.stdv, self.threshF * self.stdv, self.threshL * self.stdv
	
