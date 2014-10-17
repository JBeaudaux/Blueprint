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
from threading import Timer
import eZ430, dbus, time, math, os, sys
import statistics, analysis

class windowControl(Tkinter.Tk):

	runDemo = 0	#0=inactive ; 1=live ; 2=recorded
	recordDemo = False
	pauseDemo = True

	coord = {'xaxis':0, 'yaxis':0, 'zaxis':0, 'acc':0}

	precision = 0.02	# Time interval between samples (in sec)
	iteration = 0	# Number of iterations from the first sample
	windowSize = 1200

	moyVal = 0
	moySSE = 0
	SSE = 0


	def __init__(self,parent, option=0):
        	Tkinter.Tk.__init__(self,parent)
		self.parent = parent
		self.title('Blue print watch control center')
		ico = r"%s/%s/blueprint.gif"%(os.getcwd(), sys.argv[0][:-11])
		img = Tkinter.Image("photo", file=ico)
		self.tk.call('wm','iconphoto',self._w,img)
		
		self.detectA = analysis.anomalyDetection()
		
		self.drawall = option
		
		self.initialize()


	# Draws movement whenever activity is detected
	def drawCoordinates(self, xaxis, yaxis, zaxis, prevx, prevy, prevz, acc, prevacc, option=0):
		if self.iteration%self.windowSize == 0:
			self.canvas.delete("all")
			self.drawAxis()
		
		if self.iteration%self.windowSize < 20 or self.iteration%self.windowSize > self.windowSize-10:
			return
		
		self.canvas.create_oval(self.iteration%self.windowSize, acc, (self.iteration%self.windowSize)+1, prevacc, outline='cornflower blue', fill='cornflower blue')
		
		if self.drawall == 1:
			self.canvas.create_line(self.iteration%self.windowSize, prevx, (self.iteration%self.windowSize)+1, xaxis, fill='blue')
			self.canvas.create_line(self.iteration%self.windowSize, prevy, (self.iteration%self.windowSize)+1, yaxis, fill='red')
			self.canvas.create_line(self.iteration%self.windowSize, prevz, (self.iteration%self.windowSize)+1, zaxis, fill='green')


	def drawAxis(self):
		self.canvas.create_line(25, 25, 70, 25, fill='cornflower blue')
		self.canvas.create_text(200, 25, text='Accelerometric axials root-sum-of-square', fill='black')
		
		self.canvas.create_line(20, 10, 20, 280, fill='black')
		self.canvas.create_line(20, 10, 15, 20, fill='black')
		self.canvas.create_line(20, 10, 25, 20, fill='black')
		self.canvas.create_text(10, 150, text='Movement amplitude', fill='black', angle=90)
		
		self.canvas.create_line(20, 280, self.windowSize-10, 280, fill='black')
		self.canvas.create_line(self.windowSize-10, 280, self.windowSize-20, 275, fill='black')
		self.canvas.create_line(self.windowSize-10, 280, self.windowSize-20, 285, fill='black')
		self.canvas.create_text(float(self.windowSize)/2, 290, text='Time (s)', fill='black')
		
		iteration = 1
		i = 20
		while i<self.windowSize-20:
			self.canvas.create_line(i, 275, i, 280, fill='black')
			i+=iteration*(1/self.precision)


	# Draws anomaly detection thresholds and parameters
	def drawDetection(self, vals):
		diff, stdv, threshF, threshL = vals[0], vals[1], vals[2], vals[3]
		
		#if self.iteration%self.windowSize == 0:
		#	self.canvasAcc.delete("all")
		
		if diff != 0:
			if diff > threshF :
				self.labelVariable.set("Falling ?!? (%f > %f)"%(diff, threshF))
			elif diff < threshL:
				self.labelVariable.set("Lying ... (%f < %f)"%(diff, threshL))
			else:
				self.labelVariable.set("Normal activity ... (%f < %f < %f)"%(threshL, diff, threshF))
		#	print "Test", diff, stdv, threshF, threshL
		#	self.canvasAcc.create_oval(self.iteration%self.windowSize, 100-diff, (self.iteration%self.windowSize)+1, 100-diff+1, outline='blue', fill='blue')
		#	self.canvasAcc.create_oval(self.iteration%self.windowSize, 100-threshF, (self.iteration%self.windowSize)+1, 100-threshF+1, outline='red', fill='red')
		#	self.canvasAcc.create_oval(self.iteration%self.windowSize, 100-threshL, (self.iteration%self.windowSize)+1, 100-threshL+1, outline='green', fill='green')


	# Calculates activity to detect falls
	def calculateMovement(self):
		if self.runDemo == 1:
			data = self.watch.read()
			#print data

			# Test whether data was recorded
			if len(data) > 2:# and self.coord['xaxis'] != ord(data[0]) and self.coord['yaxis'] != ord(data[1]) and self.coord['zaxis'] != ord(data[2]):
				# Keep record on last values
				prevx = self.coord['xaxis']+125 if self.coord['xaxis']<125 else self.coord['xaxis']-125
				prevx = (prevx-50)*2
				prevy = self.coord['yaxis']+125 if self.coord['yaxis']<125 else self.coord['yaxis']-125
				prevy = (prevy-50)*2
				prevz = self.coord['zaxis']+125 if self.coord['zaxis']<125 else self.coord['zaxis']-125
				prevz = (prevz-50)*2
				prevn = self.coord['acc']

				# Save new values
				self.coord['xaxis'] = ord(data[0])
				self.coord['yaxis'] = ord(data[1])
				self.coord['zaxis'] = ord(data[2])
				self.coord['acc'] = math.sqrt((prevx*prevx)+(prevy*prevy)+(prevz*prevz))

				# Translates coordinates for plotting and analysis
				finx = ord(data[0])+125 if ord(data[0])<125 else ord(data[0])-125
				finx = (finx-50)*2
				finy = ord(data[1])+125 if ord(data[1])<125 else ord(data[1])-125
				finy = (finy-50)*2
				finz = ord(data[2])+125 if ord(data[2])<125 else ord(data[2])-125
				finz = (finz-50)*2
				norm = math.sqrt((finx*finx)+(finy*finy)+(finz*finz))

				self.iteration += 1

				self.drawCoordinates(finx, finy, finz, prevx, prevy, prevz, norm-150, prevn-150)
				
				if self.recordDemo == True:
					self.file.write('i\t%d\tx\t%d\ty\t%d\tz\t%d\tn\t%d\t\n'%(self.iteration-1, finx, finy, finz, norm))

			Timer(self.precision, self.calculateMovement, ()).start()


	def displayRecord(self):
		if self.runDemo == 2 and self.pauseDemo == False:
			line = self.file.readline()
			coupe = line.split("\t")
			if len(coupe) > 1:
				self.iteration += 1
				#self.detectFall(coupe[3], coupe[5], coupe[7], float(coupe[9]))
				self.drawCoordinates(coupe[3], coupe[5], coupe[7], self.coord['xaxis'], self.coord['yaxis'], self.coord['zaxis'], int(coupe[9])-150, self.coord['acc']-150)

				self.coord['xaxis'] = int(coupe[3])
				self.coord['yaxis'] = int(coupe[5])
				self.coord['zaxis'] = int(coupe[7])
				self.coord['acc'] = int(coupe[9])
				
				self.drawDetection(self.detectA.detectFall(int(coupe[9])-150))

				Timer(self.precision, self.displayRecord, ()).start()


	# Sets the GUI up
	def initialize(self):
		self.canvas = Tkinter.Canvas(self, height=300, width=1200, bg="white")
		self.canvas.pack(padx=5, pady=5)

		#self.canvasAcc = Tkinter.Canvas(self, height=100, width=1200, bg="white")
		#self.canvasAcc.pack(padx=5, pady=5)
		
		self.drawAxis()

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
			self.drawAxis()
			
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
				self.drawAxis()
				
				self.iteration = 0
				self.buttonDemo.set("Play recoded demo")
				self.buttonRecord.set("Back to main menu")
				self.labelVariable.set("Awaiting data")
				
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

