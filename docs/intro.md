# ASC-ODE

A package for solving ordinary differential equations based on C++ implemented by **team07**:

* Anton Zamyatin <e12223389@student.tuwien.ac.at>
* Martin Huber <martin.huber@tuwien.ac.at>
* Jimmy-Daniel Vacareanu <e11901909@student.tuwien.ac.at>
* Hannah Teis <e12120508@student.tuwien.ac.at>


## Running the Code

Clone Repository in local folder

`git clone git@github.com:jimmyvcr/nssc1-team07.git`

Then you can use cmake to build ASC-ODE:

```
mkdir build
cd build
cmake ..
make
```

### Command-line interface of `test_ode`

`test_ode` uses named options so different steppers and right-hand sides can be selected consistently:

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

## Demos and Experiments

Implicit, Explicit, Improved Euler and Crank Nicholson [HERE](EX1.md)

Electric Network Model, Automatic Differentiation, Runge Kutta methods [HERE](EX2.md)

Mechanical System (with Constraints) [HERE](EX3.md)
