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
In the following plots different time-step sizes are combined with varying end-times.
The first row corresponds to the given parameters for time-step and end-time.
The second row displays results for 10-times smaller time steps and the third row shows results for a 10-times bigger end-time.
The last row shows what a combined 10-times smaller time steps and 10-times bigger end-time results in. 

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

<img src="demos/ExplicitEuler/mass_spring_phase_plot_10*steps.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_10*steps.png" width="45%" style="display:inline-block;">

### Configuration 2
Calculation time: $t_{end} = T*10$ \
Number of steps: $n = N$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T*10}{N}$

<img src="demos/ExplicitEuler/mass_spring_phase_plot_10*tend.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_10*tend.png" width="45%" style="display:inline-block;">

### Configuration 3
Calculation time: $t_{end} = T*10$ \
Number of steps: $n = N*10$ \
Frequency: $\tau= \frac{t_{end}}{n} = \frac{T*10}{N*10}= \frac{T}{N}$

<img src="demos/ExplicitEuler/mass_spring_phase_plot_10*tend_10*steps.png" width="45%" style="display:inline-block; margin-right:5%;">
<img src="demos/ExplicitEuler/mass_spring_time_evolution_10*tend_10*steps.png" width="45%" style="display:inline-block;">

Phase plot: Should be a circle for exact solutions?

Time evolution: Should be a sinusoid for exact solutions?