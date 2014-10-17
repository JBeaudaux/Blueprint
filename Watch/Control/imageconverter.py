#!/usr/bin/env python

"""Converts recorded movements in a nice-looking graphical form."""
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


import os, sys


def createPlotFile():
	infile = open('record.data', 'r')
	offsetL = int(infile.readline().split("\t")[1])
	infile.close()
	
	offsetL = 300
	offsetR = 1300
	offsetU = 400
	offsetD = 200
	print "Offsets =", offsetL, offsetR, offsetD, offsetU
	
	outfile = open('record.gp', 'w+')
	outfile.write("set term epscairo enhanced color size 40cm,10cm\n")
	#outfile.write("set output 'record.eps'\n")
	outfile.write("set output 'recordfr.eps'\n")
	#outfile.write("set term png size 800,600\n")
	#outfile.write("set output 'record.png'\n")
	
	#outfile.write("set multiplot layout 2,1\n")
	outfile.write("set tmargin 5\n")
	
	outfile.write("unset tics\n")
	outfile.write("set key font ',25'\n")
	#outfile.write("set xlabel 'Time' font ',25'\n")
	#outfile.write("set ylabel 'Movement' font ',25'\n")
	outfile.write("set xlabel 'Temps' font ',25'\n")
	outfile.write("set ylabel 'Mouvement' font ',25'\n")
	#outfile.write("set xtics font ',20'\n")
	#outfile.write("set grid\n")
	outfile.write("set border 3\n")
	
	outfile.write("set arrow from 350,240 to 600,240 lw 5\n")
	#outfile.write("set label 'Walking' at 440,225 font ',25'\n")
	outfile.write("set label 'Marche' at 440,225 font ',25'\n")
	outfile.write("set arrow from 600,230 to 700,230 lw 5\n")
	#outfile.write("set label 'Sitting down' at 605,215 font ',25'\n")
	outfile.write("set label 'S`asseyant' at 605,215 font ',25'\n")
	outfile.write("set arrow from 700,240 to 850,240 lw 5\n")
	#outfile.write("set label 'Resting' at 745,225 font ',25'\n")
	outfile.write("set label 'Personne assise' at 720,225 font ',25'\n")
	outfile.write("set arrow from 850,230 to 1075,230 lw 5\n")
	#outfile.write("set label 'Standing up and walking' at 875,215 font ',25'\n")
	outfile.write("set label 'Personne se levant et marchant' at 850,215 font ',25'\n")
	
	outfile.write("set arrow from 1075,240 to 1130,240 lw 5\n")
	#outfile.write("set label 'Fall' at 1085,225 font ',25'\n")
	outfile.write("set label 'Chute' at 1080,225 font ',25'\n")
	outfile.write("set arrow from 1130,230 to 1300,230 lw 5\n")
	#outfile.write("set label 'Lying down innactive' at 1140,215 font ',25'\n")
	outfile.write("set label 'Au sol et innactif' at 1150,215 font ',25'\n")
	
	outfile.write("set arrow from 1120,340 to 1120,300 lw 5\n")
	outfile.write("set label 'Detection' at 1110,350 font ',25'\n")
	outfile.write("set arrow from 1240,340 to 1240,300 lw 5\n")
	#outfile.write("set label 'Device' at 1210,362 font ',25'\n")
	#outfile.write("set label 'alarm' at 1215,350 font ',25'\n")
	outfile.write("set label 'Alarme' at 1205,362 font ',25'\n")
	outfile.write("set label 'materiel' at 1200,350 font ',25'\n")
	outfile.write("set arrow from 1290,340 to 1290,300 lw 5\n")
	#outfile.write("set label 'Notify' at 1265,362 font ',25'\n")
	#outfile.write("set label 'email' at 1265,350 font ',25'\n")
	outfile.write("set label 'Notif.' at 1265,362 font ',25'\n")
	outfile.write("set label 'email' at 1265,350 font ',25'\n")
	
	outfile.write("set xrange [%d:%d]\n"%(offsetL, offsetR))
	outfile.write("set yrange [%d:%d]\n"%(offsetD, offsetU))
	#outfile.write("plot 'record.data' using 2:10 with lines lt rgb '#0099FF' lw 5 title 'Accelerations root-sum-of-squares'\n")
	outfile.write("plot 'record.data' using 2:10 with lines lt rgb '#0099FF' lw 5 title 'Moyenne quadratique des accelerations'\n")
	#outfile.write("plot 'record.data' using 2:4 with lines lw 5 title 'Effective movement'\n")
	outfile.close()
	print "ok"


createPlotFile()
os.system('gnuplot record.gp')
test = open("record.eps", 'r')
print test.readline()
test.close()

#remove('record.gp')