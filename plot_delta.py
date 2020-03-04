import matplotlib.pyplot as plt
import numpy as np


S, G = np.loadtxt('option_delta.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt('MC_deltas.txt', delimiter=',', unpack=True)

plt.plot(S, G, 'b+', label='Black Scholes delta')
plt.plot(S2, G2, 'r--', label='Monte Carlo delta')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option delta', fontsize=12)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend()
plt.show()