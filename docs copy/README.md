# ASC-ODE

A package for solving ordinary differential equations based on C++ implemented by **team07**:

* Anton Zamyatin <e12223389@student.tuwien.ac.at>
* Martin Huber <martin.huber@tuwien.ac.at>
* Jimmy-Daniel Vacareanu <e11901909@student.tuwien.ac.at>
* Hannah Teis <e12120508@student.tuwien.ac.at>


## Overview Implemented Functionality 
### Solvers
<div style="display: flex; gap: 40px;">

<div>

* Explicit Euler
* Improved Euler
* Implicit Euler
* Crank–Nicolson

</div>

<div>

* Explicit Runge–Kutta
* Implicit Runge–Kutta
* Newton Solver
* Helper Function: Automatic Differentiation

</div>

</div>



### Applications 
* Electric Network Model
* Mass Spring Simulation / Constrained Mechanical System
* Pendulum


## Quick Start

Clone Repository in local folder

`git clone git@github.com:jimmyvcr/nssc1-team07.git`

Then you can use cmake to build ASC-ODE:

```
mkdir build
cd build
cmake ..
make
```


For more details, read the [documentation](https://jimmyvcr.github.io/nssc1-team07/intro.html)

Find theory behind here: <https://jschoeberl.github.io/IntroSC/ODEs/ODEs.html>
