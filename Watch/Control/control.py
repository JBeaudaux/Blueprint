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


import eZ430, dbus, time, math, os
import sys, Tkinter
import gui
import smtplib
from smtplib import SMTPException


message="Subject: Blue print emergency message\n\nThis is an emergency message."

conf_map = {'receiver': '', 'sender': '', 'password': '', 'server': '', 'port': '', 'trigger': ''}

# Demo mode displaying the watch sensed data for public advertisement
def demoMode(extra, option=0):
	#Start the GUI	
	app = gui.windowControl(None, option)
	app.mainloop()


def callCaretake():
	print "Call caretake!!!"
	mess = "From: Blue print system <%s>\nTo: Caretaker<%s>\n%s"%(conf_map['sender'], conf_map['receiver'], message)
	
	print mess
	
	try:
		#smtpObj = smtplib.SMTP(conf_map['server'], int(conf_map['port']))
		#smtpObj.sendmail(send, recv, mess)
		print "Successfully sent email"
	except SMTPException:
		print "Error: unable to send email"


def verifyInfos():
	ok = False

	if not os.path.exists("conf.blue"):
		handleConfig(None)

	while not ok:
		ok = True

		conffile = open('conf.blue', 'r')

		# Retrieve all infos at once
		for line in conffile:
			line=line.rstrip().split(" ")
			if len(line) == 3 and line[0] in conf_map:
				conf_map[line[0]] = line[2]
		conffile.close()
		
		for it in conf_map:
			if conf_map[it] == '':
				ok = False

		# Test numerics
		if not conf_map['port'].isdigit() or not conf_map['trigger'].isdigit():
			ok = False
			print "Invalid port or trigger number"
		
		# Test emails
		if len(conf_map['sender'].split('@')) != 2 or len(conf_map['receiver'].split('@')) != 2:
			ok = False
			print "Invalid sender or receiver mail address"

		if ok == False:
			handleConfig(None)

	print "All parameters seem valid"


# Operational control mode that calls caretakers upon alert message
def controlMode(extra, option=0):
	run = 0
	
	verifyInfos()
	
	callCaretake()
	return

	# Wireless link init
	watch = eZ430.watch()

	data = watch.read()
	pastdata = data
	time.sleep(1)
	data = watch.read()
	pastdata = data

	while run == 1:
		data = watch.read()
		datalen = len(data)

		#Test for incoming data
		if datalen > 0 and data != pastdata:
			callCaretake()
			time.sleep(int(conf_map['trigger']))	#Waits for the alarm to dissipate
			data = watch.read()
			time.sleep(1)
			data = watch.read()
			data = watch.read()
			print "Alarm off"

		pastdata = data

		#Define alarm type
		#Send email to caretakers
		time.sleep(0.02)


def handleConfig(extra, option=0):
	conffile = open('conf.blue', 'w+')
	conffile.write("!!!Blue print configuration file!!!")

	line = raw_input('Enter the email address of the caretaker to be contacted\n')
	conffile.write("receiver = %s\n"%(line))

	line = raw_input('Enter the email address to use to send emergency messages with\n')
	conffile.write("sender = %s\n"%(line))

	line = raw_input('Enter the password of the email address to use to send with\n')
	conffile.write("password = %s\n"%(line))

	line = raw_input('Enter the mail server used to send with\n')
	conffile.write("server = %s\n"%(line))

	line = raw_input('Enter the port of the mail server used to send with\n')
	conffile.write("port = %s\n"%(line))

	line = raw_input('Enter the delay for the alarm to be set off (in sec)\n')
	conffile.write("trigger = %s\n"%(line))
	conffile.close()


# Provides details on the control center
def handleHelp(extra, option=0):
	print '\nBlueprint watch control center'
	print '==============================\n'
	print 'The Blue print open smart-watch is designed to detect sudden health onsets (i.e. Falls and heart failures), thus enabling preventive measures to be taken or notifying caretakers for improved and quicker emergency response. The control center is meant as a relay between the watch and the caretakers, notifying them when needed, as well as a tool for displaying the capabilities of the watch.\n'
	print 'Commands'
	print '--------\n'
	print 'control\tIn this mode, the control center handles the emergency messages sent by the watch and notifies caretakers through SMS/eMail.'
	print 'demo\tIn this mode, the control center graphically displays the watch sensed data (i.e. movement, heart rate) and subsequent interpretations.'
	print 'config\tAllow for the system configuration (setting sender and receiver infos, etc.).'
	print 'help\tEnters this very help menu'
	print ''
	print 'Options'
	print '-------\n'
	print 'demo [drawall] displays axial movements in addition to the axials root-sum-of-squares if drawall equals to 1'
	print ''


if __name__ == '__main__':
	if len(sys.argv) < 2:
		print 'Usage: sudo python control.py COMMAND [options]'
		sys.exit(-1)

	option = 0
	command_map = {'control': controlMode, 'demo': demoMode, 'config': handleConfig, 'help': handleHelp}
	command = sys.argv[1]
	if len(sys.argv) > 2:
		option = int(sys.argv[2])
	
	if command in command_map:
		command_map[command](sys.argv[2:], option)
	else:
		print 'Command not supported: {}'.format(command)
		sys.exit(-1)

