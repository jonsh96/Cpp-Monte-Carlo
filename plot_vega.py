import matplotlib.pyplot as plt
import numpy as np

S, G = np.loadtxt('option_vega.txt', delimiter=',', unpack=True)
plt.plot(S, G, 'b+', label='Option vega')

plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Option vega', fontsize=12)
plt.xlim([min(S), max(S)])
plt.grid()
plt.legend()
plt.show()