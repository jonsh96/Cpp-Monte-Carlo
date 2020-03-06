import numpy as np
import matplotlib.pyplot as plt

x = [1, 2, 3, 4, 5]
y = [1, 2, 3, 4, 5]
y.remove(y[0])
x.remove(x[0])
plt.plot(x, y, 'ro')
plt.show()