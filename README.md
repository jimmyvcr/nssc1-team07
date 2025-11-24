# ASC-ODE
A package for solving ordinary differential equations

Read the [documentation](https://tuwien-asc.github.io/ASC-ODE/intro.html)

Find theory behind here: https://jschoeberl.github.io/IntroSC/ODEs/ODEs.html

# The Group
We are *team07*
Members:
* Anton Zamyatin e12223389@student.tuwien.ac.at
* Martin Huber martin.huber@tuwien.ac.at
* Jimmy-Daniel Vacareanu e11901909@student.tuwien.ac.at
* Hannah Teis e12120508@student.tuwien.ac.at

# The Task
* Solve the exercises from the jupyterbook: [LINK](https://jschoeberl.github.io/IntroSC/ODEs/ODEs.html)
    * Jupyterbook Sections 14 - 17
    * First Exercise: Implementing the explicit Euler method [LINK](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ee.html#exercise:~:text=Implementing%20the%20explicit%20Euler%20method,-We)
    * Second Exercise: Coding the Implicit Euler method [LINK](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ie.html#:~:text=Coding%20the%20Implicit%20Euler%20method,-In)
* Push your homework into your git repository (this repository)

# How to compile?
`git clone git@github.com:jimmyvcr/nssc1-team07.git`

Then you can use cmake to build ASC-ODE:

```
mkdir build
cd build
cmake ..
make
```

Then run
```
cd build
./test_ode
cd ..
```
and to see the plots run
```
cd demos
python plotmassspring.py
```

# Exercises
## Different time-steps and larger end-times
Exact solution of mass-spring ODE results in sinusoidal oscillation over time (time evolution) and circular phase plot. The updates with explicit Euler are not (totally) energy conserving and so numerical errors accumulate over time. In the following plots different time-step sizes are combined with varying end-times. Their effect on the accuracy of the resulting numerical solution is interpreted.  
The first row corresponds to the given parameters for time-step and end-time.
The second row displays results for 10-times smaller time steps and the third row shows results for a 10-times bigger end-time.
The last row shows what a combined 10-times smaller time steps and 10-times bigger end-time results in. 

## Explicit Euler
### Base configuration
Calculation time: $t_{end} = 4*\pi = T$ \
Number of steps: $n = 100 = N$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T}{N}= \frac{4*\pi}{100}$

<img src="demos/ExplicitEuler/mass_spring_phase_plot_nomod.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_nomod.png" width="45%" style="display:inline-block;">

### Configuration 1
Calculation time: $t_{end} = T$ \
Number of steps: $n = N*10$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T}{N*10}$

When the time interval is divided into ten times more steps, the explicit Euler method produces more intermediate approximations with smaller local, numerical errors. This results in a solution that more closely matches the analytical sinusoid. This is also reflected in the plots, as the numerical solution is closer to a circle in the phase plot (reduced spiral artifacts), and a sinusiod over time (amplitude preservation).

<img src="demos/ExplicitEuler/mass_spring_phase_plot_10*steps.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_10*steps.png" width="45%" style="display:inline-block;">

### Configuration 2
Calculation time: $t_{end} = T*10$ \
Number of steps: $n = N$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T*10}{N}$

When the end-time is increased while keeping the number of steps constant, the step size becomes larger. This reduces the number of intermediate approximations and increases the local numerical error. Because explicit Euler propagates each step from the previous one, these errors accumulate over time, leading to a larger overall deviation from the exact solution. 
This can be seen in the plots: the phase plot no longer resembles a circle, and the time evolution shows an almost flat sinusoid with growing deviations toward the end (amplitude grows beyond analytical bounds).

<img src="demos/ExplicitEuler/mass_spring_phase_plot_10*tend.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_10*tend.png" width="45%" style="display:inline-block;">

### Configuration 3
Calculation time: $t_{end} = T*10$ \
Number of steps: $n = N*10$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T*10}{N*10}= \frac{T}{N}$

Increasing both the number of steps and the end-time by the same factor keeps the step size unchanged, so one might expect the results to somewhat match the original calculation. However, although the local error per step is the same, the longer simulation involves more steps, and because explicit Euler propagates errors, the solution gradually deviates more from the exact trajectory.

<img src="demos/ExplicitEuler/mass_spring_phase_plot_10*tend_10*steps.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_10*tend_10*steps.png" width="45%" style="display:inline-block;">

## Improved Euler
### Base configuration
Calculation time: $t_{end} = 4*\pi = T$ \
Number of steps: $n = 100 = N$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T}{N}= \frac{4*\pi}{100}$

<img src="demos/ImprovedEuler/mass_spring_phase_nomod.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ImprovedEuler/mass_spring_time_evolution_nomod.png" width="45%" style="display:inline-block;">

### Configuration 1
Calculation time: $t_{end} = T$ \
Number of steps: $n = N*10$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T}{N*10}$

<img src="demos/ImprovedEuler/mass_spring_phase_10steps.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ImprovedEuler/mass_spring_time_evolution_10steps.png" width="45%" style="display:inline-block;">

### Configuration 2
Calculation time: $t_{end} = T*10$ \
Number of steps: $n = N$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T*10}{N}$

You can observe a better behaviour as the Explicit Euler, still it is not behaving as it should (it is exploding to infinity).

<img src="demos/ImprovedEuler/mass_spring_phase_10tend.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ImprovedEuler/mass_spring_time_evolution_10tend.png" width="45%" style="display:inline-block;">

### Configuration 3
Calculation time: $t_{end} = T*10$ \
Number of steps: $n = N*10$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T*10}{N*10}= \frac{T}{N}$

For a well suitet Frequency the Improved Euler is producing a decent calculation.

<img src="demos/ImprovedEuler/mass_spring_phase_10tend_10steps.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ImprovedEuler/mass_spring_time_evolution_10tend_10steps.png" width="45%" style="display:inline-block;">

## Implicit Euler

<img src="demos/ImplicitEuler/mass_spring_implicit_phase_comparison.png" width="45%" style="display:inline-block;">
<img src="demos/ImplicitEuler/mass_spring_implicit_time_evo_comparison.png" width="45%" style="display:inline-block;">

### Configuration 1 ($\tau \approx 0.125$)
The time evolution plot shows the amplitude of the oscillations decaying rapidly. Meanwhile, the phase plot reveals a spiral quickly converging toward the equilibrium point (0, 0), suggesting a substantial energy loss.

### Configuration 2 ($\tau \approx 0.012$)
Reducing the time step by a factor of ten makes the solution more accurate. The time evolution plot shows oscillations that decay slowly, which preserves the system's energy better over the simulation time. The phase plot resembles a tighter spiral, approaching the perfect circle of an ideal, energy-conserving system.

### Configuration 3 ($\tau \approx 0.5$)
Using a larger time step shows the trade-off between stability and accuracy inherent to the method. The numerical damping effect becomes more visible, slowing the system's motion considerably after a single oscillation. The phase plot shows the trajectory collapsing directly toward the center. Although the solution remains stable, the accuracy is poor and does not well represent the system's physical behavior.