#!/usr/bin/env python

"""Blue print watch operational control center."""
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

import eZ430, dbus, time, math
import sys, Tkinter
import gui

#Demo mode making the movement/fall calculations on the computer
def demoMode():
	#Start the GUI	
	app = gui.windowControl(None)
	app.mainloop()
	
	#Determine position and movement
	"""while 1:
		data = watch.read()
		datalen = len(data)
		if datalen > 2: #Test whether data is recorded or not.
			print "x: %s\ty:%s\tz:%s"%(ord(data[0]),ord(data[1]),ord(data[2]))
			app.drawCoordinates(ord(data[0]),ord(data[1]),ord(data[2]))
		time.sleep(0.5)"""

#Operational control mode that calls caretakers upon alert message
def controlMode():
	run=1

	#while run == 1:
	#	data = watch.read()
	#	datalen = len(data)
		#Test for incoming data
		#Define alarm type
		#Send email to caretakers

#Wireless link init
#watch = eZ430.watch()

if len(sys.argv)>1 and sys.argv[1] == "demo":
	demoMode()
elif len(sys.argv)>1 and sys.argv[1] == "control":
	controlMode()

