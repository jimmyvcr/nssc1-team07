import numpy as np

data = np.loadtxt("../build/output_test_ode.txt", usecols=(0, 1, 2))
# print (data)

import matplotlib.pyplot as plt

plt.plot(data[:, 0], data[:, 1], label="position")
plt.plot(data[:, 0], data[:, 2], label="velocity")
plt.xlabel("time")
plt.ylabel("value")
plt.title("Mass-Spring System Time Evolution, Improved Euler (10*tend, 10*steps)")
plt.legend()
plt.grid()
plt.savefig("mass_spring_time_evolution_10tend_10steps.png")
plt.show()


plt.plot(data[:, 1], data[:, 2], label="phase plot")
plt.xlabel("position")
plt.ylabel("velocity")
plt.title("Mass-Spring System Phase Plot, Improved Euler (10*tend, 10*steps)")
plt.legend()
plt.grid()
plt.savefig("mass_spring_phase_10tend_10steps.png")
plt.show()
