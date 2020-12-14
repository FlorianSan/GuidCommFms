import getopt
import os
import string
import sys

from ivy.std_api import *
import matplotlib.pyplot as plt
import numpy as np

import tkinter as tk
from PIL import ImageTk
from PIL import Image

IVYAPPNAME = 'Horizon Artificiel'
angle = 0
fpa = 0

class SimpleApp(object):
    def __init__(self, master, filename_back, filename_center, **kwargs):
        self.master = master
        self.master.title('Horizon artificiel')
        self.filename_center = filename_center
        self.filename_back = filename_back
        self.canvas = tk.Canvas(master, width=500, height=500)
        self.canvas.pack()

        self.update = self.draw().__next__
        master.after(100, self.update)
        
        self.master.protocol("WM_DELETE_WINDOW", self.on_closing)

    def draw(self):
        image_back = Image.open(self.filename_back)
        image_center = Image.open(self.filename_center)
        global angle
        global fpa
        while True:
            tkimage_center = ImageTk.PhotoImage(image_center.rotate(angle))
            canvas_obj = self.canvas.create_image(250, 250, image=tkimage_center)
            self.canvas.move(canvas_obj, 0, fpa)
            tkimage_back = ImageTk.PhotoImage(image_back)
            canvas_obj_back = self.canvas.create_image(250, 250, image=tkimage_back)
            self.master.after_idle(self.update)
            yield
            self.canvas.delete(canvas_obj)
          
    def on_closing(self):
        IvyStop()
        root.destroy()
            
            
            
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


def changeangle(agent, *larg):
    global angle
    global fpa
    fpa = 3*np.clip(float(larg[4]), -20, 10) #15 correponds à 5°
    angle = np.clip(float(larg[6])*57.296, -35, 35)
    angle %= 360
    
def changeRollCmd(agent, *larg):
    float(larg[0])

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
    
    # binding on dedicated message : starting with 'hello ...'
    #IvyBindMsg(onSetPosition, "^anglev=([^ ]*)")
    IvyBindMsg(changeangle, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)")
    #StateVector x=0 y=0 z=0 Vp=0 fpa=10 psi=0 phi=10
    IvyBindMsg(changeRollCmd, "^APLatControl rollRate=[^ ] ")
    
    root = tk.Tk()
    app = SimpleApp(root,'background_trans.png', 'centre_trans.png')
    root.mainloop()

