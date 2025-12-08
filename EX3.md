# Exercise 3

## Examples for mass_sping

## Add Distance Constraints

Implementation of Distance Constraints in mass_spring.hpp:
DistanceConstraints class
- class variables: connectors (array of 2 connectors on which constraint is applied) and length (as constraint)  

MassSpringSystem class
- added constraints as class variable m_constraints
- basic functions: 
  - constraints(): get constraints
  - addConstraint(DistanceConstraint c): extend m_constraints vector with c and returns index of added element
  - constraints printed as part of obj

MSS_Function   
To enforce constraints if they are added

--> M distance constraints add M (scalar) Lagrange multipliers $\lambda$ as unknowns -> extend dimX with M   
--> each constraint gives one scalar equation to solve, matching the number of unknowns -> extend dimF with number of constraints 

evaluate(VectorView<double> x, VectorView<double> f): evaluates physical right hand side of mass-spring system by computing gravity + spring forces (acceleration) for every mass with given x (current mass position) and f (pre-allocated memory for force) --> $f(x)=a(x)$

--> For each constraint
- get Lagrange multiplier $\lambda$ from unknown vector
- retrieve constraint and its connectors 
- identify positions of connectors
- compute constraint force on each connector:
  $$m_i \ddot x_i =\frac{\partial}{\partial x_i} L(x, \lambda)$$
  - Lagrange function $L(x, \lambda) = -U(x) + \left< \lambda , g(x) \right>$ (potential $U(x)$ already computed in acceleration force from gravity and spring)
     $$\frac{\partial}{\partial x_i} L(x, \lambda) = \frac{\partial}{\partial x_i} \lambda g(x) = \frac{\partial}{\partial x_i} \lambda (| x - x_0 |^2 - l^2) = \pm 2\lambda*(x-x_0)$$
  - add constraint forces to forces of connectors
- compute constraint residual: 
  $$ 0 = \nabla_\lambda L(x, \lambda) $$
  $$ \nabla_\lambda L(x, \lambda) = \nabla_\lambda (-U(x) +  \lambda g(x)) = g(x)$$
  - add constraint residual to force at position corresponding to Lagrange multipliers
  - Solver adjusts $\lambda$ so that these residuals go to zero

## Implement exact derivative for MSS_Function

## Experiment with Mechanical Structures

### The Double-Pendulum on a chain

### Complex Beam structures

### Double Pendulum with springs

### Build a spinning top
