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


# Demo mode displaying the watch sensed data for public advertisement
def demoMode(extra):
	#Start the GUI	
	app = gui.windowControl(None)
	app.mainloop()



# Operational control mode that calls caretakers upon alert message
def controlMode(extra):
	run=1

	# Wireless link init
	#watch = eZ430.watch()

	#while run == 1:
	#	data = watch.read()
	#	datalen = len(data)
		#Test for incoming data
		#Define alarm type
		#Send email to caretakers



# Provides details on the control center
def handleHelp(extra):
	print '\nBlueprint watch control center'
	print '==============================\n'
	print 'The Blue print open smart-watch is designed to detect sudden health onsets (i.e. Falls and heart failures), thus enabling preventive measures to be taken or notifying caretakers for improved and quicker emergency response. The control center is meant as a relay between the watch and the caretakers, notifying them when needed, as well as a tool for displaying the capabilities of the watch.\n'
	print 'Commands'
	print '--------\n'
	print 'control : In this mode, the control center handles the emergency messages sent by the watch and notifies caretakers through SMS/eMail;\n'
	print 'demo : In this mode, the control center graphically displays the watch sensed data (i.e. movement, heart rate) and subsequent interpretations.\n'
	#print 'Options'
	#print '-------\n'



command_map = {
    'control': controlMode,
    'demo': demoMode,
    'help': handleHelp
}


if __name__ == '__main__':
	if len(sys.argv) < 2:
		print 'Usage: sudo python control.py COMMAND [options]'
		sys.exit(-1)

	command = sys.argv[1]
	if command in command_map:
		command_map[command](sys.argv[2:])
	else:
		print 'Command not supported: {}'.format(command)
		sys.exit(-1)

