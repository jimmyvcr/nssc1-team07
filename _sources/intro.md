# ASC-ODE

A package for solving ordinary differential equations

Read the [documentation](https://tuwien-asc.github.io/ASC-ODE/intro.html)

Find theory behind here: <https://jschoeberl.github.io/IntroSC/ODEs/ODEs.html>

## The Group

We are *team07*
Members:

* Anton Zamyatin <e12223389@student.tuwien.ac.at>
* Martin Huber <martin.huber@tuwien.ac.at>
* Jimmy-Daniel Vacareanu <e11901909@student.tuwien.ac.at>
* Hannah Teis <e12120508@student.tuwien.ac.at>

## The Task

* Solve the exercises from the jupyterbook: [LINK](https://jschoeberl.github.io/IntroSC/ODEs/ODEs.html)
  * Exercise 1: Jupyterbook Sections 14 - 17
    * [Explicit and Improved Euler](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ee.html#exercise)
    * [Implicit Euler and Crank-Nicolson](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ie.html#excercises) (first 3 tasks)
  * Exercise 2: Jupyterbook Sections 17 - 19
    * [Model electric network with ODE](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ie.html#excercises) (last task)
    * [Automatic Differentiation part1](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ad.html#exercises)
    * [Automatic Differentiation part2](https://jschoeberl.github.io/IntroSC/ODEs/implementation_ad.html#exercise-test-the-autodiff-class-for-the-pendulum)
    * [Runge-Kutta methods](https://jschoeberl.github.io/IntroSC/ODEs/RungeKutta.html#exercises)
  * Exercise 3: Jupyter book Section 20
    * The Exercise can be found [HERE](https://jschoeberl.github.io/IntroSC/ODEs/mechanical.html)
    * [Mass-spring System with Newmark branch](https://jschoeberl.github.io/IntroSC/ODEs/mechanical.html#mass-spring-system)
* Push your homework into your git repository (this repository)

## Running the Code

### How to compile?

`git clone git@github.com:jimmyvcr/nssc1-team07.git`

Then you can use cmake to build ASC-ODE:

```
mkdir build
cd build
cmake ..
make
```

To run the main demo with default settings (mass-spring system, explicit Euler), execute

```
cd build
./test_ode --stepper exp_euler --rhs mass_spring
```

### Command-line interface of `test_ode`

`test_ode` now uses named options so different steppers and right-hand sides can be selected consistently:

```
./test_ode --stepper <name> [--rhs <mass_spring|electric_network>] [--stages <int>] [--n-factor <double>] [--t-end-factor <double>]
```

* `--stepper` (required): `exp_euler`, `impl_euler`, `impr_euler`, `crank_nicolson`, `impl_rk_gauss_legendre`, `impl_rk_gauss_radau`.
* `--rhs` (default `mass_spring`): switches the modeled system.
`electric_network` is reserved for the upcoming circuit model.
* `--stages`: required only for the Gauss–Legendre/Gauss–Radau implicit RK steppers; specifies the number of stages.
* `--n-factor`, `--t-end-factor`: scale the baseline number of steps (`N=100`) and simulation horizon (`T_end=4π`).

Both `--opt value` and `--opt=value` syntaxes are supported. Example (3-stage Gauss–Legendre with increased resolution and end-time):

```
./test_ode --rhs mass_spring --stepper impl_rk_gauss_legendre --stages 3 --n-factor 10 --t-end-factor 10
```

### Plotting

First, navigate to the root directory and install all Python dependencies using `uv`:

```bash
uv sync
```

After running the `test_ode` programm as described above, you can plot the results saved to `build/<system>_<stepper>_*.txt` by calling:

```bash
uv run demos/plot_ode_results.py build/<system>_<stepper>_*.txt
```

The plots will be saved to `demos/<Stepper>/<system>_phase_*.png` and `demos/<Stepper>/<system>_time_evolution_*.png`.

## Exercise 1

You can find the implementation of Exercise 1 [HERE](EX1.md)

## Exercise 2

You can find the implementation of Exercise 2 [HERE](EX2.md)

## Exercise 3

You can find the implementation of Exercise 3 [HERE](EX3.md)
