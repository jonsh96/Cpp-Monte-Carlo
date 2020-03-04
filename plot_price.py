import matplotlib.pyplot as plt
import numpy as np

S, G = np.loadtxt('option_price.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt('MC_prices.txt', delimiter=',', unpack=True)

plt.plot(S, G, 'b+', label='Black Scholes option price')
plt.plot(S2, G2, 'r--', label='Monte Carlo option price')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option price', fontsize=12)
plt.xlim([min(S), max(S)])


plt.grid()
plt.legend()
plt.show()