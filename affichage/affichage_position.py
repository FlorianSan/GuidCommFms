import getopt
import os
import string
import sys

from ivy.std_api import *
import matplotlib.pyplot as plt
import numpy as np
import time


IVYAPPNAME = 'Affichage position'

X = [0]
Y = [0]

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
    pass


def ondieproc(agent, _id):
    pass

    
def changePos(agent, *larg):
    global X
    global Y
    X.append(float(larg[0]))
    Y.append(float(larg[1]))
    
   
        
    

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
    

    IvyBindMsg(changePos, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)")


    plt.ion()

    figure, a = plt.subplots(figsize=(5,5))
    
    
   
    
    a.set_xlim([-10000, 10000])
    a.set_ylim([-10000, 10000])
    
    a.grid(True,linestyle='-',color='0.75')
    line1, = a.plot([X],[Y], markersize=1, color="red", marker=".")
    a.set_xlabel("Position X")
    a.set_ylabel("Position Y")
    figure.canvas.set_window_title('Affichage position avion')
    
    while(True):
        line1.set_xdata(X)
        line1.set_ydata(Y)
        figure.canvas.draw()
        figure.canvas.flush_events()
        #a.relim()
        #a.autoscale_view()
        #if msvcrt.kbhit() and msvcrt.getch() == chr(27).encode():
        #    aborted = True
        #    break
       
    
