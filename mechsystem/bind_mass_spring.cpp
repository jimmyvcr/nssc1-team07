#include <sstream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include "mass_spring.hpp"
#include "Newmark.hpp"
#include "timestepper.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<Mass<3>>);
PYBIND11_MAKE_OPAQUE(std::vector<Fix<3>>);
PYBIND11_MAKE_OPAQUE(std::vector<Spring>);
PYBIND11_MAKE_OPAQUE(std::vector<DistanceConstraint>);

PYBIND11_MODULE(mass_spring, m) {
    m.doc() = "mass-spring-system simulator"; 

    py::class_<Mass<2>> (m, "Mass2d")
          .def_property("mass",
                    [](Mass<2> & m) { return m.mass; },
                    [](Mass<2> & m, double mass) { m.mass = mass; })
      .def_property_readonly("pos",
                             [](Mass<2> & m) { return m.pos.data(); });

      ;
      
    m.def("Mass", [](double m, std::array<double,2> p)
    {
      return Mass<2>{m, { p[0], p[1] }};
    });

    
    py::class_<Mass<3>> (m, "Mass3d")
      .def_property("mass",
                    [](Mass<3> & m) { return m.mass; },
                    [](Mass<3> & m, double mass) { m.mass = mass; })
      .def_property_readonly("pos",
                             [](Mass<3> & m) { return m.pos.data(); });
    ;

    m.def("Mass", [](double m, std::array<double,3> p)
    {
      return Mass<3>{m, { p[0], p[1], p[2] }};
    });



    py::class_<Fix<2>> (m, "Fix2d")
      .def_property_readonly("pos",
                             [](Fix<2> & f) { return f.pos.data(); });

    m.def("Fix", [](std::array<double,2> p)
    {
      return Fix<2>{ { p[0], p[1] } };
    });


    
    py::class_<Fix<3>> (m, "Fix3d")
      .def_property_readonly("pos",
                             [](Fix<3> & f) { return f.pos.data(); });
    
    m.def("Fix", [](std::array<double,3> p)
    {
      return Fix<3>{ { p[0], p[1], p[2] } };
    });

    py::class_<Connector> connector(m, "Connector");
    
    py::enum_<Connector::CONTYPE>(connector, "CONTYPE")
        .value("FIX", Connector::FIX)
        .value("MASS", Connector::MASS)
        .export_values();

    connector
      .def(py::init<>())
      .def_readwrite("type", &Connector::type)
      .def_readwrite("nr", &Connector::nr);

    py::class_<Spring> (m, "Spring")
      .def(py::init<double, double, std::array<Connector,2>>())
      .def_property_readonly("connectors",
                             [](Spring & s) { return s.connectors; })
      ;
    
    py::class_<DistanceConstraint> (m, "DistanceConstraint")
      .def(py::init<double, std::array<Connector,2>>())
      .def_property_readonly("connectors", 
                             [](DistanceConstraint & s) { return s.connectors; })
;

    
    py::bind_vector<std::vector<Mass<3>>>(m, "Masses3d");
    py::bind_vector<std::vector<Fix<3>>>(m, "Fixes3d");
    py::bind_vector<std::vector<Spring>>(m, "Springs"); 
    py::bind_vector<std::vector<DistanceConstraint>>(m, "Constraints");       
    
    
    py::class_<MassSpringSystem<2>> (m, "MassSpringSystem2d")
      .def(py::init<>())
      .def("add", [](MassSpringSystem<2> & mss, Mass<2> m) { return mss.addMass(m); })
      ;
      

    // This makes the C++ Vector class available in Python and allows it to behave like a NumPy array.
    py::class_<Vector<double>>(m, "Vector", py::buffer_protocol())
      .def(py::init<size_t>())
      .def("__len__", [](const Vector<double> &v) { return v.size(); })
      .def("__getitem__", [](const Vector<double> &v, size_t i) { return v(i); })
      .def("__setitem__", [](Vector<double> &v, size_t i, double val) { v(i) = val; })
      .def_buffer([](Vector<double> &v) -> py::buffer_info {
          return py::buffer_info(
              v.data(),
              sizeof(double),
              py::format_descriptor<double>::format(),
              1,
              { v.size() },
              { sizeof(double) }
          );
      }); 
        
    py::class_<MassSpringSystem<3>> (m, "MassSpringSystem3d")
      .def(py::init<>())
      .def("getState", &MassSpringSystem<3>::getState, py::return_value_policy::copy)
      .def("setState", &MassSpringSystem<3>::setState)
      .def("setState", [](MassSpringSystem<3> &self, Vector<double> &x) {
          // Create dummy vectors for velocity and acceleration since Python doesn't have them
          Vector<> dx(x.size());
          dx = 0.0;
          Vector<> ddx(x.size());
          ddx = 0.0;
          // Call the real C++ function with all three required arguments
          self.setState(x, dx, ddx);
      })
      .def("setState", [](MassSpringSystem<3> &self, Vector<double> &x, Vector<double> &dx, Vector<double> &ddx) {
          self.setState(x, dx, ddx);
      })
      .def("__str__", [](MassSpringSystem<3> & mss) {
        std::stringstream sstr;
        sstr << mss;
        return sstr.str();
      })
      .def("add", [](MassSpringSystem<3> & mss, DistanceConstraint c) { return mss.addConstraint(c); })
      .def_property_readonly("constraints", [](MassSpringSystem<3> & mss) -> auto& { return mss.constraints(); })
      .def_property("gravity", [](MassSpringSystem<3> & mss) { return mss.getGravity(); },
                    [](MassSpringSystem<3> & mss, std::array<double,3> g) { mss.setGravity(Vec<3>{g[0],g[1],g[2]}); })
      .def("add", [](MassSpringSystem<3> & mss, Mass<3> m) { return mss.addMass(m); })
      .def("add", [](MassSpringSystem<3> & mss, Fix<3> f) { return mss.addFix(f); })
      .def("add", [](MassSpringSystem<3> & mss, Spring s) { return mss.addSpring(s); })
      .def_property_readonly("masses", [](MassSpringSystem<3> & mss) -> auto& { return mss.masses(); })
      .def_property_readonly("fixes", [](MassSpringSystem<3> & mss) -> auto& { return mss.fixes(); })
      .def_property_readonly("springs", [](MassSpringSystem<3> & mss) -> auto& { return mss.springs(); })
      .def("__getitem__", [](MassSpringSystem<3> mss, Connector & c) {
        if (c.type==Connector::FIX) return py::cast(mss.fixes()[c.nr]);
        else return py::cast(mss.masses()[c.nr]);
      })
      
      .def("getState", [] (MassSpringSystem<3> & mss) {
        Vector<> x(3*mss.masses().size());
        Vector<> dx(3*mss.masses().size());
        Vector<> ddx(3*mss.masses().size());
        mss.getState (x, dx, ddx);
        return x;
      })

      .def("simulate", [](MassSpringSystem<3> & mss, double tend, size_t steps) {
        size_t n_mass = 3 * mss.masses().size();
        size_t n_con = mss.constraints().size();
        size_t dim = n_mass + n_con;

        Vector<> x(dim);
        Vector<> dx(dim);
        Vector<> ddx(dim);
        x = 0.0; dx = 0.0; ddx = 0.0;

        mss.getState (x.range(0, n_mass), dx.range(0, n_mass), ddx.range(0, n_mass));

        auto mss_func = std::make_shared<MSS_Function<3>> (mss);
        // Mass matrix: Identity for masses (0..n_mass), Zero for constraints (n_mass..dim)
        auto mass = std::make_shared<Projector> (dim, 0, n_mass);

        SolveODE_Alpha(tend, steps, 0.5, x, dx, ddx, mss_func, mass);

        mss.setState (x.range(0, n_mass), dx.range(0, n_mass), ddx.range(0, n_mass));  
    });

    // Expose the base class for the ODE right-hand-side function
    py::class_<NonlinearFunction, std::shared_ptr<NonlinearFunction>>(m, "NonlinearFunction");

    // Expose the MSS_Function which is a type of NonlinearFunction
    py::class_<MSS_Function<3>, NonlinearFunction, std::shared_ptr<MSS_Function<3>>>(m, "MSS_Function3d")
        .def(py::init<MassSpringSystem<3>&>());

    // Expose the base class for time-stepping methods
    py::class_<TimeStepper>(m, "TimeStepper");

    // Expose the ExplicitEuler solver
    py::class_<ExplicitEuler, TimeStepper>(m, "ExplicitEuler")
        .def(py::init<std::shared_ptr<NonlinearFunction>>())
        .def("DoStep", [](ExplicitEuler &self, double tau, Vector<double> &y) {
            self.DoStep(tau, y);
        });

    py::class_<ImplicitEuler, TimeStepper>(m, "ImplicitEuler")
        .def(py::init<std::shared_ptr<NonlinearFunction>>())
        .def("DoStep", [](ImplicitEuler &self, double tau, Vector<double> &y) {
            self.DoStep(tau, y);
        });
  
    
}
