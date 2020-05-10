import matplotlib.pyplot as plt
import numpy as np
import os


"""
ABOUT: 
- Reads price values from text files
- Plots the values
- Saves figures as .png
"""
# Gets folder above relative to this file
dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# Reads title from file and creates the filename
title = open(dir_path + '\\data\\title.txt').read()

# Reads values from files
S1, G1 = np.loadtxt(dir_path + '\\data\\mc_exact_stddev.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt(dir_path + '\\data\\MC_stddev.txt', delimiter=',', unpack=True)
S3, G3 = np.loadtxt(dir_path + '\\data\\mc_exact_stddev_VR.txt', delimiter=',', unpack=True)
S4, G4 = np.loadtxt(dir_path + '\\data\\MC_stddev_VR.txt', delimiter=',', unpack=True)

# Plots values
plt.plot(S1, G1, 'r+-', label='Exact simulation, with variance reduction')
plt.plot(S2, G2, 'b--', label='Euler method, with variance reduction')
plt.plot(S3, G3, 'k+-', label='Exact simulation, no variance reduction')
plt.plot(S4, G4, 'g--', label='Euler method, no variance reduction')
plt.legend(fontsize=14)
plt.title(title, fontsize=18)
plt.xlabel('Initial stock price', fontsize=14)
plt.ylabel('Standard deviation', fontsize=14)
plt.xlim([min(S1), max(S1)])
plt.grid()
plt.legend()
plt.show()