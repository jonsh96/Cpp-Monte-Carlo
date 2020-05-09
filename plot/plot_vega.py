import matplotlib.pyplot as plt
import numpy as np
import os

dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

title = open(dir_path + '\\data\\title.txt').read()
S, G = np.loadtxt(dir_path + '\\data\\option_vega.txt', delimiter=',', unpack=True)
plt.plot(S, G, 'b+', label='Option vega')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option vega', fontsize=12)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend()
plt.show()