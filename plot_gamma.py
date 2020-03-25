import matplotlib.pyplot as plt
import numpy as np

title = open("title.txt").read()
S, G = np.loadtxt('option_gamma.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt('MC_gammas.txt', delimiter=',', unpack=True)
S3, G3 = np.loadtxt('MC_exact_gammas.txt', delimiter=',', unpack=True)

plt.plot(S, G, 'b+-', label='Fair option gamma')
plt.plot(S2, G2, 'r+-', label='Monte Carlo Euler method gamma')
plt.plot(S3, G3, 'g+-', label='Monte Carlo exact simulation gamma')
plt.title(title)

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option gamma', fontsize=12)
plt.xlim([min(S)+1, max(S)-1])
plt.grid()
plt.legend()
plt.show()