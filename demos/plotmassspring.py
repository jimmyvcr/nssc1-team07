import numpy as np
import sys

data = np.loadtxt("../build/output_test_ode.txt", usecols=(0, 1, 2))
# print (data)

import matplotlib.pyplot as plt

# checks for input files and get the list of filenames to plot
if len(sys.argv) < 2:
    print("Usage: python plotmassspring.py <file1.txt> <file2.txt> ...")
    sys.exit(1)
filenames = sys.argv[1:]

plt.figure(1)

# loops over every filename provided
for file_path in filenames:
    data = np.loadtxt(file_path, usecols=(0, 1, 2))
    plt.plot(data[:, 0], data[:, 1], label=f"position ({file_path})")
    plt.plot(data[:, 0], data[:, 2], label=f"velocity ({file_path})")

plt.xlabel("time")
plt.ylabel("value")
plt.title("Mass-Spring System Time Evolution Comparison")
plt.legend()
plt.grid()
plt.savefig("mass_spring_time_comparison.png")
plt.show()

plt.figure(2)

# loops over every filename again for the phase plot
for file_path in filenames:
    data = np.loadtxt(file_path, usecols=(0, 1, 2))
    plt.plot(data[:, 1], data[:, 2], label=f"phase plot ({file_path})")

plt.xlabel("position")
plt.ylabel("velocity")
plt.title("Mass-Spring System Phase Plot Comparison")
plt.legend()
plt.grid()
plt.savefig("mass_spring_phase_comparison.png")
plt.show()
