import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("C:\\Users\\Jón Sveinbjörn\\source\\repos\\WbsProject\\paths.txt", delimiter=',')
plt.figure()
row_nr = 0
for row in data:
    plt.plot(data[row_nr, :])
    row_nr += 1

plt.xlim([0, len(data[0, :])-1])
plt.grid(True)
plt.show()