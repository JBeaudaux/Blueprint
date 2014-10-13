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
import eZ430, dbus, time, math, os, sys
import statistics

class windowControl(Tkinter.Tk):

	runDemo = 0	#0=inactive ; 1=live ; 2=recorded
	recordDemo = False
	pauseDemo = True

	coord={'xaxis':0, 'yaxis':0, 'zaxis':0, 'acc':0}

	precision=0.02	# Time interval between samples (in sec)
	iteration=0	# Number of iterations from the first sample
	windowSize = 1200



	def __init__(self,parent):
        	Tkinter.Tk.__init__(self,parent)
		self.parent = parent
		self.title('Blue print watch control center')
		ico = r"%s/%s/blueprint.gif"%(os.getcwd(), sys.argv[0][:-11])
		img = Tkinter.Image("photo", file=ico)
		self.tk.call('wm','iconphoto',self._w,img)
		
		self.initialize()



	# Prints movement whenever activity is detected
	def drawCoordinates(self, xaxis, yaxis, zaxis, prevx, prevy, prevz, acc):
		if self.iteration%self.windowSize == 0:
			self.canvas.delete("all")

		self.canvas.create_line(self.iteration%self.windowSize, prevx, (self.iteration%self.windowSize)+1, xaxis, fill='blue')
		self.canvas.create_line(self.iteration%self.windowSize, prevy, (self.iteration%self.windowSize)+1, yaxis, fill='red')
		self.canvas.create_line(self.iteration%self.windowSize, prevz, (self.iteration%self.windowSize)+1, zaxis, fill='green')
		#self.canvas.create_oval(self.iteration, acc, self.iteration+1, acc+1, outline='black', fill='black')



	# Calculates acceleration to detect fall patterns
	def detectFall(self, xaxis, yaxis, zaxis, prevx, prevy, prevz):
		norm = math.sqrt((xaxis*xaxis)+(yaxis*yaxis)+(zaxis*zaxis))
		#print "I %d\t X %d\t Y %d\t Z %d\t N %d"%(self.iteration%self.windowSize, xaxis, yaxis, zaxis, norm)
		return norm



	# Calculates activity to detect falls
	def calculateMovement(self):
		if self.runDemo == 1:
			data = self.watch.read()
			#print data

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
				
				if self.recordDemo == True:
					self.file.write('i\t%d\tx\t%d\ty\t%d\tz\t%d\tn\t%d\t\n'%(self.iteration-1, finx, finy, finz, norm))

			Timer(self.precision, self.calculateMovement, ()).start()



	def displayRecord(self):
		if self.runDemo == 2 and self.pauseDemo == False:
			line = self.file.readline()
			coupe = line.split("\t")
			if len(coupe) > 1:
				self.iteration += 1
				self.drawCoordinates(coupe[3], coupe[5], coupe[7], self.coord['xaxis'], self.coord['yaxis'], self.coord['zaxis'], coupe[9])

				self.coord['xaxis'] = int(coupe[3])
				self.coord['yaxis'] = int(coupe[5])
				self.coord['zaxis'] = int(coupe[7])

				Timer(self.precision, self.displayRecord, ()).start()



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

		self.buttonDemo = Tkinter.StringVar()
		ButtonOn = Tkinter.Button(self, textvariable=self.buttonDemo, command=self.DemoButtonClick)
		ButtonOn.pack(padx=10, pady=10)
		self.buttonDemo.set("Start live demo")

		self.buttonRecord = Tkinter.StringVar()
		ButtonOn = Tkinter.Button(self, textvariable=self.buttonRecord, command=self.RecordButtonClick)
		ButtonOn.pack(padx=10, pady=10)
		self.buttonRecord.set("Load recorded demo")



	def DemoButtonClick(self):
        	if self.runDemo == 0:
			self.runDemo = 1
			self.iteration = 0
			self.canvas.delete("all")
			self.buttonDemo.set("Record current demo")
			self.buttonRecord.set("Back to main menu")
			self.labelVariable.set("Awaiting events")

			#Wireless link init
			if not hasattr(self, 'watch'):
				self.watch = eZ430.watch()

			Timer(self.precision, self.calculateMovement, ()).start()
		elif self.runDemo == 1 and self.recordDemo == False:
			self.recordDemo = True
			self.buttonDemo.set("Pause recording demo")
			if not hasattr(self, 'file'):
				self.file = open('record.data', 'w+')

			print "Recording to file record.data"
			#self.watch.stop()
		elif self.runDemo == 1 and self.recordDemo == True:
			self.recordDemo = False
			self.buttonDemo.set("Record current demo")
		elif self.runDemo == 2 and self.pauseDemo == True:
			self.buttonDemo.set("Pause recoded demo")
			self.pauseDemo = False
			self.displayRecord()
		elif self.runDemo == 2 and self.pauseDemo == False:
			self.buttonDemo.set("Play recoded demo")
			self.pauseDemo = True
			


	def RecordButtonClick(self):
		if self.runDemo == 0:
			if os.path.exists("record.data"):
				print "Loading recorded demo..."

				self.runDemo = 2
				self.canvas.delete("all")
				self.iteration = 0
				self.buttonDemo.set("Play recoded demo")
				self.buttonRecord.set("Back to main menu")
				self.labelVariable.set("Playing recorded demo")
				
				self.file = open('record.data', 'r')
				Timer(self.precision, self.displayRecord, ()).start()
			else:
				print "No record to load with"
		elif self.runDemo > 0:
			self.runDemo = 0

			self.recordDemo = False
			if hasattr(self, 'file'):
				self.file.close()

			self.buttonDemo.set("Start live demo")
			self.buttonRecord.set("Load recorded demo")
			self.labelVariable.set("Ready to start")

