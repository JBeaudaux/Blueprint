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

#Wireless link init
watch = eZ430.watch()

x=0
y=0
z=0

while 1:
	data = watch.read()
	datalen = len(data)
	if datalen > 2:
		print "x: %s\ty:%s\tz:%s"%(ord(data[0]),ord(data[1]),ord(data[2]))
	time.sleep(1)
