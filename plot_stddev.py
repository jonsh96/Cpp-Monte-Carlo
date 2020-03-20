import matplotlib.pyplot as plt
import numpy as np

title = open("title.txt").read()
S1, G1 = np.loadtxt('mc_exact_stddev.txt', delimiter=',', unpack=True)
S2, G2 = np.loadtxt('MC_stddev.txt', delimiter=',', unpack=True)
S3, G3 = np.loadtxt('mc_exact_stddev_VR.txt', delimiter=',', unpack=True)
S4, G4 = np.loadtxt('MC_stddev_VR.txt', delimiter=',', unpack=True)

plt.plot(S1, G1, 'r+-', label='Exact simulation, with variance reduction')
plt.plot(S2, G2, 'b--', label='Euler method, with variance reduction')
plt.plot(S3, G3, 'k+-', label='Exact simulation, no variance reduction')
plt.plot(S4, G4, 'g--', label='Euler method, no variance reduction')

plt.title(title)
plt.xlabel('Initial stock price', fontsize=12)
plt.ylabel('Standard deviation', fontsize=12)
plt.xlim([min(S1), max(S1)])


plt.grid()
plt.legend()
plt.show()