import matplotlib.pyplot as plt
import numpy as np


S, G = np.loadtxt('option_gamma.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt('MC_gammas.txt', delimiter=',', unpack=True)

plt.plot(S, G, 'b+', label='Black Scholes gamma')
plt.plot(S2, G2, 'r--', label='Monte Carlo gamma')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option gamma', fontsize=12)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend()
plt.show()