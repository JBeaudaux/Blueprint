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

import Tkinter

class windowControl(Tkinter.Tk):
	def __init__(self,parent):
        	Tkinter.Tk.__init__(self,parent)
		self.parent = parent

		self.initialize()

	def initialize(self):
		self.grid()

		self.entry = Tkinter.Entry(self)
		self.entry.grid(column=0,row=0,sticky='EW')

		button = Tkinter.Button(self, text=u"Start demo mode", command=self.OnButtonClick)
		button.grid(column=1,row=1)
		self.entry.bind("<Return>", self.OnPressEnter)

		button = Tkinter.Button(self, text=u"Monitor movement", command=self.OnButtonClick)
		button.grid(column=2,row=1)
		self.entry.bind("<Return>", self.OnPressEnter)

		self.labelVariable = Tkinter.StringVar()
		label = Tkinter.Label(self, textvariable=self.labelVariable, anchor="w", fg="black", bg="white")
		label.grid(column=1, row=1, columnspan=1, sticky='EW')
		self.labelVariable.set("Awaiting events")

		img = Tkinter.PhotoImage(file="test.gif")  # reference PhotoImage in local variable
		button2 = Tkinter.Button(self, image=img)
		button.img = img
		button2.grid()

		self.grid_columnconfigure(0,weight=1)
		self.resizable(True,False)

	def OnButtonClick(self):
        	self.labelVariable.set("Demo mode started")

	def OnPressEnter(self,event):
        	self.labelVariable.set("You pressed enter !")
