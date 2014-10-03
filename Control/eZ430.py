#!/usr/bin/env python

"""Blue print watch and control center communication core."""
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

import serial

class watch():
        def __init__(self, dev = "/dev/ttyACM0", deb = 50):
                self.dev = dev
                self.deb = deb
		self.start()
	def start(self):
                self.conn = serial.Serial(self.dev, 115200, timeout = 1)
                self.write("\xFF\x07\x03")
	def stop(self):
		self.write("\xFF\x09\x03")
		self.conn.close()
        def write(self,msg):
                self.conn.write(msg)
        def read(self, len = 7):
                self.conn.write("\xFF\x08\x07\x00\x00\x00\x00")
		#print self.conn.read(len)
                return self.conn.read(len)
        def debounce(self):
                i=self.deb
                while i:
                        self.read()
                        i-=1
	
