#include <cerrno>
#include <cstdlib>
#include <fstream> 
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <nonlinfunc.hpp>
#include <timestepper.hpp>

using namespace ASC_ode;


class MassSpring : public NonlinearFunction
{
private:
  double mass;
  double stiffness;

public:
  MassSpring(double m, double k) : mass(m), stiffness(k) {}

  size_t dimX() const override { return 2; }
  size_t dimF() const override { return 2; }
  
  void evaluate (VectorView<double> x, VectorView<double> f) const override
  {
    f(0) = x(1);
    f(1) = -stiffness/mass*x(0);
  }
  
  void evaluateDeriv (VectorView<double> x, MatrixView<double> df) const override
  {
    df = 0.0;
    df(0,1) = 1;
    df(1,0) = -stiffness/mass;
  }
};

int main(int argc, char* argv[])
{
  auto print_usage = [argv]() {
    std::cerr << "Usage: " << argv[0] << " <stepper> [N_factor] [T_end_factor]\n";
    std::cerr << "  stepper      : exp_euler | impl_euler | impr_euler | crank_nicolson\n";
    std::cerr << "  N_factor     : optional, scales default steps N=100 (default 1.0)\n";
    std::cerr << "  T_end_factor : optional, scales default T_end = 4*pi (default 1.0)\n";
  };

  auto parse_factor = [](const char* text, const char* name) {
    errno = 0;
    char* end = nullptr;
    double value = std::strtod(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE)
      throw std::invalid_argument(std::string("Invalid ") + name + ": " + text);
    return value;
  };

  if (argc < 2 || argc > 4) {
    print_usage();
    return 1;
  }

  std::string stepper_name = argv[1];
  double n_fact = 1.0;
  double tend_fact = 1.0;
  bool n_overridden = false;
  bool t_overridden = false;

  try {
    if (argc >= 3) {
      n_fact = parse_factor(argv[2], "N_factor");
      n_overridden = true;
    }
    if (argc == 4) {
      tend_fact = parse_factor(argv[3], "T_end_factor");
      t_overridden = true;
    }
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    print_usage();
    return 1;
  }

  if (n_fact <= 0.0 || tend_fact <= 0.0) {
    std::cerr << "N_factor and T_end_factor must be positive." << std::endl;
    return 1;
  }

  double tend = 4*M_PI*tend_fact;
  int steps = 100*n_fact;
  if (steps <= 0) {
    std::cerr << "Resulting number of steps must be positive." << std::endl;
    return 1;
  }
  double tau = tend/steps;

  Vector<> y = { 1, 0 };  // initializer list
  auto rhs = std::make_shared<MassSpring>(1.0, 1.0);

  std::unique_ptr<TimeStepper> stepper;
  if (stepper_name == "exp_euler") {
    stepper = std::make_unique<ExplicitEuler>(rhs);
  }
  else if (stepper_name == "impl_euler") {
    stepper = std::make_unique<ImplicitEuler>(rhs);
  }
  else if (stepper_name == "impr_euler") {
    stepper = std::make_unique<ImprovedEuler>(rhs);
  }
  else if (stepper_name == "crank_nicolson") {
    stepper = std::make_unique<CrankNicolson>(rhs);
  }
  else {
    std::cerr << "Unknown stepper '" << stepper_name << "'." << std::endl;
    print_usage();
    return 1;
  }

  const double default_factor = 1.0;
  const double eps = 1e-12;
  bool t_modified = t_overridden && std::abs(tend_fact - default_factor) > eps;
  bool n_modified = n_overridden && std::abs(n_fact - default_factor) > eps;

  std::ostringstream namebuilder;
  namebuilder << "mass_spring_" << stepper_name;
  if (!t_modified && !n_modified) {
    namebuilder << "_nomod";
  }
  else {
    if (t_modified)
      namebuilder << "_" << tend_fact << "tend";
    if (n_modified)
      namebuilder << "_" << n_fact << "steps";
  }
  namebuilder << ".txt";
  std::string outfilename = namebuilder.str();

  std::ofstream outfile(outfilename);
  if (!outfile) {
    std::cerr << "Failed to open output file '" << outfilename << "'." << std::endl;
    return 1;
  }

  std::cout << "Running simulation with tau = " << tau << ", outputting to " << outfilename << std::endl;

  // std::cout << 0.0 << "  " << y(0) << " " << y(1) << std::endl;
  outfile << 0.0 << "  " << y(0) << " " << y(1) << std::endl;

  for (int i = 0; i < steps; i++)
  {
    stepper->DoStep(tau, y);

     // std::cout << (i+1) * tau << "  " << y(0) << " " << y(1) << std::endl;
     outfile << (i+1) * tau << "  " << y(0) << " " << y(1) << std::endl;
  }

  std::cout << "test_ode.cpp finished!" << std::endl;
  return 0;
}
