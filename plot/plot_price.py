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
filename = ("figures\\" + title[0:title.find('simulations')]+'price.png').replace(' ', '_')

# Reads values from files
S, G = np.loadtxt(dir_path + '\\data\\option_price.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt(dir_path + '\\data\\MC_prices.txt', delimiter=',', unpack=True)
S3, G3 = np.loadtxt(dir_path + '\\data\\MC_exact_prices.txt', delimiter=',', unpack=True)

# Plots values and saves the figure as a .png file
plt.plot(S, G, 'b+-', label='Fair option price')
plt.plot(S2, G2, 'r+-', label='Monte Carlo Euler method price')
plt.plot(S3, G3, 'g+-', label='Monte Carlo exact simulation price')
plt.title(title, fontsize=18)
plt.xlabel('Initial stock price', fontsize=14)
plt.ylabel('Option price', fontsize=14)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend(fontsize=14)
figure = plt.gcf()
figure.set_size_inches(16, 9)
plt.savefig(filename, dpi=100, bbox_inches='tight')
plt.show()

