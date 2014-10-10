#!/usr/bin/env python

"""Blue print watch operational control center GUI."""
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

import Tkinter
import time
from threading import Timer
import eZ430, dbus, time, math

class windowControl(Tkinter.Tk):

	runDemo = False
	coord={'xaxis':0, 'yaxis':0, 'zaxis':0, 'acc':0}

	precision=0.02	# Time interval between samples (in sec)
	iteration=0	# Number of iterations from the first sample



	def __init__(self,parent):
        	Tkinter.Tk.__init__(self,parent)
		self.parent = parent
		self.title('Blue print watch control center')
		self.initialize()



	# Prints movement whenever activity is detected
	def drawCoordinates(self, xaxis, yaxis, zaxis, prevx, prevy, prevz, acc):
		if self.iteration > 1200:
			self.iteration = 0
			self.canvas.delete("all")

		self.canvas.create_line(self.iteration, prevx, self.iteration+1, xaxis, fill='blue')
		self.canvas.create_line(self.iteration, prevy, self.iteration+1, yaxis, fill='red')
		self.canvas.create_line(self.iteration, prevz, self.iteration+1, zaxis, fill='green')
		#self.canvas.create_oval(self.iteration, acc, self.iteration+1, acc+1, outline='black', fill='black')



	# Calculates acceleration to detect fall patterns
	def detectFall(self, xaxis, yaxis, zaxis, prevx, prevy, prevz):
		norm = math.sqrt((xaxis*xaxis)+(yaxis*yaxis)+(zaxis*zaxis))
		print "X %d\t Y %d\t Z %d\t N %d"%(xaxis, yaxis, zaxis, norm)
		return norm



	# Calculates activity to detect falls
	def calculateMovement(self):
		if self.runDemo == True:
			data = self.watch.read()

			# Test whether data was recorded
			if len(data) > 2 and self.coord['xaxis'] != ord(data[0]) and self.coord['yaxis'] != ord(data[1]) and self.coord['zaxis'] != ord(data[2]):
				# Keep record on last values
				prevx = self.coord['xaxis']+125 if self.coord['xaxis']<125 else self.coord['xaxis']-125
				prevx = (prevx-50)*2
				prevy = self.coord['yaxis']+125 if self.coord['yaxis']<125 else self.coord['yaxis']-125
				prevy = (prevy-50)*2
				prevz = self.coord['zaxis']+125 if self.coord['zaxis']<125 else self.coord['zaxis']-125
				prevz = (prevz-50)*2

				# Save new values
				self.coord['xaxis'] = ord(data[0])
				self.coord['yaxis'] = ord(data[1])
				self.coord['zaxis'] = ord(data[2])

				# Translates coordinates for plotting and analysis
				finx = ord(data[0])+125 if ord(data[0])<125 else ord(data[0])-125
				finx = (finx-50)*2
				finy = ord(data[1])+125 if ord(data[1])<125 else ord(data[1])-125
				finy = (finy-50)*2
				finz = ord(data[2])+125 if ord(data[2])<125 else ord(data[2])-125
				finz = (finz-50)*2

				# Calculate acceleration
				norm = self.detectFall(finx, finy, finz, prevx, prevy, prevz)

				self.iteration += 1

				self.drawCoordinates(finx, finy, finz, prevx, prevy, prevz, norm)
				# TODO Make possibility to record current Demo (or play it)
				#self.recordDemo(finx, finy, finz, prevx, prevy, prevz, norm)

			Timer(self.precision, self.calculateMovement, ()).start()



	# Sets the GUI up
	def initialize(self):
		self.canvas = Tkinter.Canvas(self, height=300, width=1200, bg="white")
		self.canvas.pack(padx=5, pady=5)

		#self.canvasY = Tkinter.Canvas(self, height=200, width=1400)
		#self.canvasY.pack(padx=5, pady=5)

		#self.canvasZ = Tkinter.Canvas(self, height=200, width=1400)
		#self.canvasZ.pack(padx=5, pady=5)

		self.labelVariable = Tkinter.StringVar()
		label = Tkinter.Label(self, textvariable=self.labelVariable, anchor="w", fg="black", bg="white")
		label.pack(padx=5, pady=5)
		self.labelVariable.set("Ready to start")

		self.buttonVariable = Tkinter.StringVar()
		ButtonOn = Tkinter.Button(self, textvariable=self.buttonVariable, command=self.OnButtonClick)
		ButtonOn.pack(padx=10, pady=10)
		self.buttonVariable.set("Start demo mode")



	def OnButtonClick(self):
        	if self.runDemo == False:
			self.runDemo = True
			self.buttonVariable.set("Stop demo mode")
			self.labelVariable.set("Awaiting events")

			#Wireless link init
			self.watch = eZ430.watch()

			Timer(self.precision, self.calculateMovement, ()).start()
		else:
			self.runDemo = False
			self.buttonVariable.set("Start demo mode")
			self.labelVariable.set("Ready to start")
			#self.watch.stop()

