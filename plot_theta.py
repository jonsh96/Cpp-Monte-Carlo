import matplotlib.pyplot as plt
import numpy as np

S, G = np.loadtxt('option_theta.txt', delimiter=',', unpack=True)
plt.plot(S, G, label='Option theta')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option theta', fontsize=12)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend()
plt.show()