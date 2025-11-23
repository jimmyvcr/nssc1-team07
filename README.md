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