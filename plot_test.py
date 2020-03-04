import numpy as np
import matplotlib.pyplot as plt

title = open("title.txt").read()
data = np.loadtxt("paths.txt", delimiter=',')
plt.figure()
row_nr = 0
for row in data:
    plt.plot(data[row_nr, :])
    row_nr += 1

plt.title(title)
plt.xlabel('Number of time steps', fontsize=12)
plt.ylabel('Asset price', fontsize=12)
plt.xlim([0, len(data[0, :])-1])
plt.grid(True)
plt.show()