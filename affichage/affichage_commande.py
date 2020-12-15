import getopt
import os
import string
import sys

from ivy.std_api import *
import matplotlib.pyplot as plt
import numpy as np
import time


IVYAPPNAME = 'Affichage commande'

p =0
nz =0 

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

    
def changeRollCmd(agent, *larg):
    global p
    p = float(larg[0])*57.296
   
    
def changeNz(agent, *larg):
    global nz
    nz = float(larg[0])
    
def on_close(event):
    pass
        
    

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
    
    #StateVector x=0 y=0 z=0 Vp=0 fpa=10 psi=0 phi=10
    IvyBindMsg(changeRollCmd, "^APLatControl rollRate=(.*)")
    #IPLatControl rollRate=1
    
    IvyBindMsg(changeNz, "^IPNzControl nz=(.*)")
    #IPNzControl nz=1
    
    plt.ion()

    figure, a = plt.subplots(figsize=(8,8))
    figure.canvas.mpl_connect('close_event', on_close)
    a.set_xlabel("Facteur de charge nz",fontsize=12, labelpad=210)
    a.set_ylabel("Vitesse de roulis (°/s)",fontsize=12, labelpad=210)
    
    a.spines['left'].set_position('center')
    a.spines['bottom'].set_position('center')

    # Eliminate upper and right axes
    a.spines['right'].set_color('none')
    a.spines['top'].set_color('none')
    
    a.set_xlim([-3.5, 3.5])
    a.set_ylim([-1.5, 1.5])
    
    a.grid(True,linestyle='-',color='0.75')
    line1, = a.plot([p],[nz], markersize=5, color="red", marker="s")
    figure.canvas.set_window_title('Affichage commande avion')
    while(True):
        line1.set_xdata(p)
        line1.set_ydata(nz)
        figure.canvas.draw()
        figure.canvas.flush_events()
        #if msvcrt.kbhit() and msvcrt.getch() == chr(27).encode():
        #    aborted = True
        #    break
       
    
