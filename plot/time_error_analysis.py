from statistics import mean
import numpy as np
import matplotlib.pyplot as plt


def complexity(n, t):
    h, k = best_fit_slope_and_intercept(np.log(n), np.log(t))
    a = np.exp(k)
    return a, h


def best_fit_slope_and_intercept(xs, ys):
    h = (((mean(xs) * mean(ys)) - mean(xs * ys)) /
         ((mean(xs) * mean(xs)) - mean(xs * xs)))
    k = mean(ys) - h * mean(xs)
    return h, k


def time_plot(n, y):
    plt.plot(n, y, '+-')
    plt.grid(True, which="both")
    plt.xlim([min(N), max(N)])
    plt.xlabel('Number of simulations')
    plt.ylabel('Time elapsed')


def error_plot(n, y):
    plt.plot(n, y, '+-')
    plt.grid(True, which="both")
    plt.xscale('log')
    plt.xlim([min(n), max(n)])
    plt.xlabel('Number of simulations')
    plt.ylabel('Maximum pricing error')


N = [10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 163840, 327680]
european_Euler_time = [0.0237206, 0.0255406, 0.0726509, 0.127999, 0.257006, 0.385934, 1.37282, 1.50367, 2.46535,
                       5.24142, 10.1908, 15.1865, 45.2882, 87.3545, 117.253, 314.515]
european_Exact_time = [0.0178763, 0.046874, 0.120472, 0.196204, 0.305918, 0.699898, 1.43784, 1.87832, 3.91102, 7.3806,
                       15.3002, 31.8701, 62.9145, 120.765, 181.451, 524.777]
european_Euler_max_error = [7.71273, 3.69363, 0.32404, 0.0897466, 1.0019, 0.34801, 0.674294, 0.385727, 0.198897,
                            0.163609, 0.0212755, 0.0271782, 0.0231619, 0.0331916, 0.00618167, 0.00510076]
european_Exact_max_error = [7.72516, 3.73183, 0.375555, 0.0866982, 1.01528, 0.352993, 0.677048, 0.389299, 0.201086,
                            0.163756, 0.0200938, 0.0279351, 0.0236344, 0.0327875, 0.00628543, 0.0050719]
asian_Euler_time = [0.0147769, 0.0315528, 0.0876078, 0.13283, 0.249323, 0.538955, 0.848913, 1.74639, 3.89538, 7.23833,
                    14.4309, 30.334, 65.5701, 108.585, 159.164, 329.12]
asian_Exact_time = [0.0128261, 0.0176813, 0.0430418, 0.06783, 0.178546, 0.323732, 0.675347, 1.08533, 2.3104, 4.20908,
                    7.89166, 16.3747, 42.9212, 67.5838, 229.552, 587.689]
asian_Euler_max_error = [3.73275, 1.26945, 0.333781, 0.281497, 0.167654, 0.141123, 0.108136, 0.135941, 0.0865451,
                         0.110479, 0.0378996, 0.026195, 0.0230628, 0.0298633, 0.00714853, 0.0061245]
asian_Exact_max_error = [3.74902, 1.29502, 0.31541, 0.267739, 0.171983, 0.140947, 0.109082, 0.136795, 0.0872566,
                         0.109799, 0.0379321, 0.0258486, 0.0224782, 0.03013, 0.00718541, 0.00613467]

print("European Euler time complexity: ", complexity(N, european_Euler_time))
print("European exact time complexity: ", complexity(N, european_Exact_time))
print("Asian Euler time complexity: ", complexity(N, asian_Euler_time))
print("Asian exact time complexity: ", complexity(N, asian_Exact_time))

time_plot(N, european_Euler_time)
time_plot(N, european_Exact_time)
time_plot(N, asian_Euler_time)
time_plot(N, asian_Exact_time)
plt.legend(['European Euler time', 'European exact time', 'Asian Euler time', 'Asian exact time'])
plt.show()

error_plot(N, european_Euler_max_error)
error_plot(N, european_Exact_max_error)
error_plot(N, asian_Euler_max_error)
error_plot(N, asian_Exact_max_error)
plt.legend(['European Euler pricing error', 'European exact pricing error',
            'Asian Euler pricing error', 'Asian exact pricing error'])
plt.show()
