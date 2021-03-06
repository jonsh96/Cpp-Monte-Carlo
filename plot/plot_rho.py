""""
ABOUT:
- Reads rho values from text files
- Plots the values
- Saves figures as .png

- This did not end up in the final product as simulating the paths to generate rho is not particularly interesting

import matplotlib.pyplot as plt
import numpy as np
import os

dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

title = open(dir_path + '\\data\\title.txt').read()
S, G = np.loadtxt(dir_path + '\\option_rho.txt', delimiter=',', unpack=True)
plt.plot(S, G, label='Option rho')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option rho', fontsize=12)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend()
plt.show()
"""