import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("Legendre/legendre.csv", header=0)

x = df['x']
plt.figure(figsize=(10, 6))
for n in range(6):
    plt.plot(x, df[f'P{n}'], label=f'P{n}(x)')
plt.title('Legendre Polynomials P0 to P5')
plt.xlabel('x')
plt.ylabel('P_n(x)')
plt.legend()
plt.grid()
plt.savefig('Legendre/legendre_polynomials.png')
plt.show()

plt.figure(figsize=(10, 6))
for n in range(6):
    plt.plot(x, df[f'dP{n}_dx'], label=f'dP{n}/dx')
plt.title('Derivatives of Legendre Polynomials dP0/dx to dP5/dx')
plt.xlabel('x')
plt.ylabel('dP_n/dx')
plt.legend()
plt.grid()
plt.savefig('Legendre/legendre_polynomials_derivatives.png')
plt.show()
