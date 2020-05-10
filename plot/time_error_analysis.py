from statistics import mean
import numpy as np
import matplotlib.pyplot as plt
import os

"""
ABOUT: 
- Reads time and error values from text files
- Plots the value
- Calculates time complexity coefficients
"""


def complexity(n, t):
    # Returns the complexity coefficients
    h, k = best_fit_slope_and_intercept(np.log(n), np.log(t))
    a = np.exp(k)
    return a, h


def best_fit_slope_and_intercept(xs, ys):
    # Returns the coefficient of the best fitting slope
    h = (((mean(xs) * mean(ys)) - mean(xs * ys)) /
         ((mean(xs) * mean(xs)) - mean(xs * xs)))
    k = mean(ys) - h * mean(xs)
    return h, k


def time_plot(n, y):
    # Plots the time measurements
    plt.plot(n, y, 'o--')
    plt.grid(True, which="both")
    plt.xlim([min(N), max(N)])
    plt.xlabel('Number of simulations', fontsize=16)
    plt.ylabel('Time elapsed', fontsize=16)


def error_plot(n, y):
    # Plots the error measurements
    plt.plot(n, y, 'o--')
    plt.grid(True, which="both")
    plt.xscale('log')
    plt.xlim([min(n), max(n)])
    plt.xlabel('Number of simulations', fontsize=16)
    plt.ylabel('Maximum pricing error', fontsize=16)

# Gets folder above relative to this file
dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# Reads the title and parameters from file
title = open(dir_path + '\\data\\title.txt').read()
params = title[title.find("(Smin"):len(title)]

# Filenames
euro_call_path = dir_path + '\\data\\call_style_0_measurements.txt'
euro_put_path = dir_path + '\\data\\put_style_0_measurements.txt'
asia_call_1_path = dir_path + '\\data\\call_style_1_measurements.txt'
asia_put_1_path = dir_path + '\\data\\put_style_1_measurements.txt'
asia_call_2_path = dir_path + '\\data\\call_style_2_measurements.txt'
asia_put_2_path = dir_path + '\\data\\put_style_2_measurements.txt'

# Loads data from text files
N, euro_call_euler_time, euro_call_euler_err, euro_call_exact_time, euro_call_exact_err \
    = np.loadtxt(euro_call_path, delimiter=',', unpack=True)
N, euro_put_euler_time, euro_put_euler_err, euro_put_exact_time, euro_put_exact_err \
    = np.loadtxt(euro_put_path, delimiter=',', unpack=True)
N, asia_call_1_euler_time, asia_call_1_euler_err, asia_call_1_exact_time, asia_call_1_exact_err \
    = np.loadtxt(asia_call_1_path, delimiter=',', unpack=True)
N, asia_put_1_euler_time, asia_put_1_euler_err, asia_put_1_exact_time, asia_put_1_exact_err \
    = np.loadtxt(asia_put_1_path, delimiter=',', unpack=True)
N, asia_call_2_euler_time, asia_call_2_euler_err, asia_call_2_exact_time, asia_call_2_exact_err \
    = np.loadtxt(asia_call_2_path, delimiter=',', unpack=True)
N, asia_put_2_euler_time, asia_put_2_euler_err, asia_put_2_exact_time, asia_put_2_exact_err \
    = np.loadtxt(asia_put_2_path, delimiter=',', unpack=True)

# Calculates the time complexity for the call options
print("\nCall options:")
print("European Euler time complexity: ", complexity(N, euro_call_euler_time))
print("European exact time complexity: ", complexity(N, euro_call_exact_time))
print("Arithmetic Asian Euler time complexity: ", complexity(N, asia_call_1_euler_time))
print("Arithmetic Asian exact time complexity: ", complexity(N, asia_call_1_exact_time))
print("Geometric Asian Euler time complexity: ", complexity(N, asia_call_2_euler_time))
print("Geometric Asian exact time complexity: ", complexity(N, asia_call_2_exact_time))

# Plotting the time measurements
time_plot(N, euro_call_euler_time)
time_plot(N, euro_call_exact_time)
time_plot(N, asia_call_1_euler_time)
time_plot(N, asia_call_1_exact_time)
time_plot(N, asia_call_2_euler_time)
time_plot(N, asia_call_2_exact_time)
plt.title('Time plot for call options\n'+params, fontsize=20)
plt.legend(['European Euler time', 'European exact time', 'Asian Arithmetic Euler time','Asian Arithmetic exact time',
            'Asian Geometric Euler time', 'Asian Geometric exact time'], fontsize=16)
figure = plt.gcf()
figure.set_size_inches(16, 9)
# Saves the figure as a .png file
plt.savefig(dir_path + '\\figures\\call_time_plot.png', dpi=100, bbox_inches='tight')
plt.show()

# Plotting the error measurements
error_plot(N, euro_call_euler_err)
error_plot(N, euro_call_exact_err)
error_plot(N, asia_call_1_euler_err)
error_plot(N, asia_call_1_exact_err)
error_plot(N, asia_call_2_euler_err)
error_plot(N, asia_call_2_exact_err)
plt.title('Error plot for call option\n' + params, fontsize=20)
plt.legend(['European Euler maximum error', 'European exact maximum error', 'Asian Arithmetic Euler maximum error',
            'Asian Arithmetic exact maximum error', 'Asian Geometric Euler maximum error',
            'Asian Geometric exact maximum error'], fontsize=16)
figure = plt.gcf()
figure.set_size_inches(16, 9)
# Saves the figure as a .png file
plt.savefig(dir_path + '\\figures\\call_error_plot.png', dpi=100, bbox_inches='tight')
plt.show()

# Calculates the time complexity for the put options
print("\nPut options:")
print("European Euler time complexity: ", complexity(N, euro_put_euler_time))
print("European exact time complexity: ", complexity(N, euro_put_exact_time))
print("Arithmetic Asian Euler time complexity: ", complexity(N, asia_put_1_euler_time))
print("Arithmetic Asian exact time complexity: ", complexity(N, asia_put_1_exact_time))
print("Geometric Asian Euler time complexity: ", complexity(N, asia_put_2_euler_time))
print("Geometric Asian exact time complexity: ", complexity(N, asia_put_2_exact_time))

# Plotting the time measurements
time_plot(N, euro_put_euler_time)
time_plot(N, euro_put_exact_time)
time_plot(N, asia_put_1_euler_time)
time_plot(N, asia_put_1_exact_time)
time_plot(N, asia_put_2_euler_time)
time_plot(N, asia_put_2_exact_time)
plt.title('Time plot for put options\n' + params, fontsize=20)
plt.legend(['European Euler time', 'European exact time', 'Asian Arithmetic Euler time','Asian Arithmetic exact time',
            'Asian Geometric Euler time', 'Asian Geometric exact time'], fontsize=16)
figure = plt.gcf()
figure.set_size_inches(16, 9)
# Saves the figure as a .png file
plt.savefig(dir_path + '\\figures\\put_time_plot.png', dpi=100, bbox_inches='tight')
plt.show()

# Plotting the error measurements
error_plot(N, euro_put_euler_err)
error_plot(N, euro_put_exact_err)
error_plot(N, asia_put_1_euler_err)
error_plot(N, asia_put_1_exact_err)
error_plot(N, asia_put_2_euler_err)
error_plot(N, asia_put_2_exact_err)
plt.title('Error plot for put option\n' + params, fontsize=20)
plt.legend(['European Euler maximum error', 'European exact maximum error', 'Asian Arithmetic Euler maximum error',
            'Asian Arithmetic exact maximum error', 'Asian Geometric Euler maximum error',
            'Asian Geometric exact maximum error'], fontsize=16)
figure = plt.gcf()
figure.set_size_inches(16, 9)
# Save the figure as a .png file
plt.savefig(dir_path + '\\figures\\put_error_plot.png', dpi=100, bbox_inches='tight')
plt.show()

