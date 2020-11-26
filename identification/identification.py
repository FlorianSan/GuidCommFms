#!/usr/bin/env python
""" pyhello.py : very simple hello world python program using ivycpy
with a local main loop
"""
import getopt
import os
import string
import sys
import math

from ivy.std_api import *
import matplotlib.pyplot as plt
import numpy as np
IVYAPPNAME = 'pyIdentification'

xtk = []
roll = []
heading = []


def lprint(fmt, *arg):
    print(IVYAPPNAME + ': ' + fmt % arg)


def usage(scmd):
    lpathitem = string.split(scmd, '/')
    fmt = '''Usage: %s [-h] [-b IVYBUS | --ivybus=IVYBUS]
    where
    \t-h provides the usage message;
    \t-b IVYBUS | --ivybus=IVYBUS allow to provide the IVYBUS string in the form
    \t adresse:port eg. 127.255.255.255:2010
    '''
    print(fmt % lpathitem[-1])


def oncxproc(agent, connected):
    if connected == IvyApplicationDisconnected:
        lprint('Ivy application %r was disconnected', agent)
    else:
        lprint('Ivy application %r was connected', agent)
    lprint('currents Ivy application are [%s]', IvyGetApplicationList())


def ondieproc(agent, _id):
    lprint('received the order to die from %r with id = %d', agent, _id)





def onSetPosition(agent, *larg):
	xtk.append(float(larg[1]))
	roll.append(float(larg[6]))
	heading.append(float(larg[5]))
	print(float(larg[0]))
	
def commande(agent, *larg):
    if(float(larg[0]) == 0):
        IvySendMsg("APLatControl rollRate=0")
    if(float(larg[0]) == 2):
        IvySendMsg("APLatControl rollRate=1") #de 2s à 3s 1°/s
    if(float(larg[0]) == 2.5):
        IvySendMsg("APLatControl rollRate=0") #de 3s à 4s 0°/s angle cst
    if(float(larg[0]) == 4):
        IvySendMsg("APLatControl rollRate=-1") #de 4s à 4.8s -1°/s 
    if(float(larg[0]) == 4.8):
	    IvySendMsg("APLatControl rollRate=0") #de 4.8s à 5.5s -1°/s 
    if(float(larg[0]) == 5.5):
        IvySendMsg("APLatControl rollRate=1") #de 7s à 8s 0°/s angle cst
    if(float(larg[0]) == 5.8):
        IvySendMsg("APLatControl rollRate=0") #de 4.8s à 5.5s -1°/s 
    if(float(larg[0]) == 10):
        IvyStop()
        t = []
        for i in range(len(xtk)):
            t.append(i*0.1)
            
        with open("file.txt", "w") as f:
            for i in range(len(xtk)):
                if(heading[i] >= math.pi):
                    f.write(str(t[i]) + ";" + str(xtk[i]) + ";" + str(roll[i]) + ";" + str(-((2*math.pi) - heading[i])) +"\n") 
                else:
                    f.write(str(t[i]) + ";" + str(xtk[i]) + ";" + str(roll[i]) + ";" + str(heading[i]) +"\n") 
        print("export Ok")
	 
        
	

if __name__ == '__main__':
    # initializing ivybus and isreadymsg
    sivybus = ''
    sisreadymsg = '[%s is ready]' % IVYAPPNAME
    # getting option
    try:
        optlist, left_args = getopt.getopt(sys.argv[1:], 'hb:', ['ivybus='])
    except getopt.GetoptError:
        # print help information and exit:
        usage(sys.argv[0])
        sys.exit(2)
    for o, a in optlist:
        if o in ('-h', '--help'):
            usage(sys.argv[0])
            sys.exit()
        elif o in ('-b', '--ivybus'):
            sivybus = a
    if sivybus:
        sechoivybus = sivybus
    elif 'IVYBUS' in os.environ:
        sechoivybus = os.environ['IVYBUS']
    else:
        sechoivybus = 'ivydefault'
    lprint('Ivy will broadcast on %s ', sechoivybus)

    # initialising the bus
    IvyInit(IVYAPPNAME,     # application name for Ivy
            sisreadymsg,    # ready message
            0,              # main loop is local (ie. using IvyMainloop)
            oncxproc,       # handler called on connection/disconnection
            ondieproc)      # handler called when a <die> message is received

    # starting the bus
    # Note: env variable IVYBUS will be used if no parameter or empty string
    # is given ; this is performed by IvyStart (C)
    IvyStart(sivybus)
    
    IvySendMsg("InitStateVector x=0 y=0 z=0 Vp=70 fpa=0 psi=0 phi=0")
    # binding on dedicated message : starting with 'hello ...'
    IvyBindMsg(onSetPosition, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)")
    IvyBindMsg(commande, "^Time t=(.*)")
    #APLatControl rollRate=0.1

    lprint('%s doing IvyMainLoop', IVYAPPNAME)

    IvyMainLoop()


