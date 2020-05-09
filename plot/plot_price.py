import matplotlib.pyplot as plt
import numpy as np
import os

dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

title = open(dir_path + '\\data\\title.txt').read()
S, G = np.loadtxt(dir_path + '\\data\\option_price.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt(dir_path + '\\data\\MC_prices.txt', delimiter=',', unpack=True)
S3, G3 = np.loadtxt(dir_path + '\\data\\MC_exact_prices.txt', delimiter=',', unpack=True)

plt.plot(S, G, 'b+-', label='Fair option price')
plt.plot(S2, G2, 'r+-', label='Monte Carlo Euler method price')
plt.plot(S3, G3, 'g+-', label='Monte Carlo exact simulation price')

plt.title(title)
plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option price', fontsize=12)
plt.xlim([min(S), max(S)])


plt.grid()
plt.legend()
plt.show()