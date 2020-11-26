#!/usr/bin/env python
""" 

"""

import matplotlib.pyplot as plt

	
t = []
xtk = []
roll = []
heading =[]
with open("file.txt", "r") as f:
        for line in f:
	        liste = line.split('\t')
	        t.append(liste[0])
	        xtk.append(liste[1])
	        roll.append(liste[2])
	        heading.append(liste[3].split('\n')[0])
plt.plot(t, xtk, t, roll, t, heading)
plt.ylabel('test')
plt.show()

