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
#include <RungeKutta.hpp>

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

// DONE: replace stub with actual electric network model once available
class ElectricNetwork : public NonlinearFunction
{
private:
    double m_R, m_C, m_omega;

public:
    // Constructor to store RC circuit parameters
    ElectricNetwork(double R, double C, double omega) 
      : m_R(R), m_C(C), m_omega(omega) {}

    // Our state vector y = (Uc, t) has 2 dimensions
    size_t dimX() const override { return 2; }
    size_t dimF() const override { return 2; }

    // This implements y' = f(y) for the autonomous system
    void evaluate (VectorView<double> x, VectorView<double> f) const override {
        // x(0) = Uc, x(1) = t
        double invRC = 1.0 / (m_R * m_C);
        
        // f(0) is the derivative of Uc
        f(0) = -invRC * x(0) + invRC * std::cos(m_omega * x(1));
        
        // f(1) is the derivative of t
        f(1) = 1.0;
    }

    // This implements the Jacobian matrix of f
    void evaluateDeriv (VectorView<double> x, MatrixView<double> df) const override {
        // x(0) = Uc, x(1) = t
        df = 0.0;
        double invRC = 1.0 / (m_R * m_C);

        // Row 0: Derivatives of f(0)
        // d(f(0))/d(x(0))
        df(0, 0) = -invRC;
        // d(f(0))/d(x(1))
        df(0, 1) = -invRC * m_omega * std::sin(m_omega * x(1));

        // Row 1: Derivatives of f(1)
        // d(f(1))/d(x(0)) = 0 and d(f(1))/d(x(1)) = 0, already set by df = 0.0
    }
};

int main(int argc, char* argv[])
{
  auto print_usage = [argv]() {
    std::cerr << "Usage: " << argv[0] << " --stepper <name> [--rhs <system>] [--stages <int>] [--n-factor <double>] [--t-end-factor <double>]\n";
    std::cerr << "  --stepper        exp_euler | impl_euler | impr_euler | crank_nicolson | impl_rk_gauss_legendre | impl_rk_gauss_radau\n";
    std::cerr << "  --rhs            mass_spring | electric_network (default mass_spring)\n";
    std::cerr << "  --stages         required for impl_rk_gauss_legendre / impl_rk_gauss_radau (positive integer)\n";
    std::cerr << "  --n-factor       optional, scales default steps N=100 (default 1.0)\n";
    std::cerr << "  --t-end-factor   optional, scales default T_end = 4*pi (default 1.0)\n";
    std::cerr << "Arguments accept either '--opt value' or '--opt=value' forms.\n";
  };

  auto parse_factor = [](const char* text, const char* name) {
    errno = 0;
    char* end = nullptr;
    double value = std::strtod(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE)
      throw std::invalid_argument(std::string("Invalid ") + name + ": " + text);
    return value;
  };

  auto parse_stages = [](const char* text) {
    errno = 0;
    char* end = nullptr;
    long value = std::strtol(text, &end, 10);
    if (end == text || *end != '\0' || errno == ERANGE || value <= 0)
      throw std::invalid_argument(std::string("Invalid stages: ") + text);
    return static_cast<int>(value);
  };

  if (argc == 1) {
    print_usage();
    return 1;
  }

  std::string stepper_name;
  auto needs_stages = [](const std::string& name) {
    return name == "impl_rk_gauss_legendre" || name == "impl_rk_gauss_radau";
  };
  int stages = 0;
  bool stages_overridden = false;

  double n_fact = 1.0;
  double tend_fact = 1.0;
  bool n_overridden = false;
  bool t_overridden = false;
  std::string rhs_name = "mass_spring";

  auto normalize_option = [](const std::string& opt) {
    if (opt.rfind("--", 0) == 0)
      return opt.substr(2);
    return opt;
  };

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      print_usage();
      return 0;
    }

    std::string key = arg;
    std::string value;
    auto eq_pos = arg.find('=');
    if (eq_pos != std::string::npos) {
      key = arg.substr(0, eq_pos);
      value = arg.substr(eq_pos + 1);
    } else {
      if (i + 1 >= argc) {
        std::cerr << "Missing value for option '" << arg << "'." << std::endl;
        print_usage();
        return 1;
      }
      value = argv[++i];
    }

    key = normalize_option(key);

    try {
      if (key == "stepper") {
        stepper_name = value;
      }
      else if (key == "rhs") {
        rhs_name = value;
      }
      else if (key == "stages") {
        stages = parse_stages(value.c_str());
        stages_overridden = true;
      }
      else if (key == "n-factor") {
        n_fact = parse_factor(value.c_str(), "N_factor");
        n_overridden = true;
      }
      else if (key == "t-end-factor") {
        tend_fact = parse_factor(value.c_str(), "T_end_factor");
        t_overridden = true;
      }
      else {
        std::cerr << "Unknown option '--" << key << "'." << std::endl;
        print_usage();
        return 1;
      }
    }
    catch (const std::exception& err) {
      std::cerr << err.what() << std::endl;
      print_usage();
      return 1;
    }
  }

  if (stepper_name.empty()) {
    std::cerr << "Missing required --stepper option." << std::endl;
    print_usage();
    return 1;
  }

  if (needs_stages(stepper_name) && !stages_overridden) {
    std::cerr << stepper_name << " requires --stages <int>." << std::endl;
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

  Vector<> y = { 0, 0 };  // Initial state: Uc=0, t=0
  std::shared_ptr<NonlinearFunction> rhs;
  if (rhs_name == "mass_spring") {
    rhs = std::make_shared<MassSpring>(1.0, 1.0);
  }
  else if (rhs_name == "electric_network") {
    // DONE: instantiate ElectricNetwork with proper parameters once implemented
    const double omega = 1.0;
    // Using parameters R=1, C=1 from the exercise
    rhs = std::make_shared<ElectricNetwork>(1.0, 1.0, omega);
  }
  else {
    std::cerr << "Unknown RHS '" << rhs_name << "'." << std::endl;
    print_usage();
    return 1;
  }

  std::unique_ptr<TimeStepper> stepper;
  std::string stepper_tag = stepper_name;
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
  else if (stepper_name == "exp_rk") {
    // TODO: script for ExplicitRungeKutta here
  }
  else if (stepper_name == "impl_rk_gauss_legendre") {
    if (!stages_overridden) {
      std::cerr << "Legendre IRK requires a stages argument." << std::endl;
      return 1;
    }

    Matrix<> a(stages, stages);
    Vector<> b(stages), c(stages);

    if (stages == 2) {
      // Example: reuse predefined Gauss-Legendre tableau
      a = Gauss2a;
      b = Gauss2b;
      c = Gauss2c;
    }
    else if (stages == 3) {
      // Example: take tabulated nodes and derive coefficients programmatically
      c = Gauss3c;
      auto [a_tmp, b_tmp] = ComputeABfromC(c);
      a = a_tmp;
      b = b_tmp;
    }
    else {
      // General construction: compute nodes (c) via Gauss-Legendre and build a,b
      Vector<> quad_weights(stages);
      GaussLegendre(c, quad_weights);
      auto [a_tmp, b_tmp] = ComputeABfromC(c);
      a = a_tmp;
      b = b_tmp;
    }

    stepper = std::make_unique<ImplicitRungeKutta>(rhs, a, b, c);
    stepper_tag = stepper_name + "_s" + std::to_string(stages);
  } 
  else if (stepper_name == "impl_rk_gauss_radau") {
    if (!stages_overridden) {
      std::cerr << "Radau IRK requires a stages argument." << std::endl;
      return 1;
    }

    Vector<> c(stages), weights(stages);
    GaussRadau(c, weights);
    auto [a_tmp, b_tmp] = ComputeABfromC(c);
    Matrix<> a = a_tmp;
    Vector<> b = b_tmp;
    stepper = std::make_unique<ImplicitRungeKutta>(rhs, a, b, c);
    stepper_tag = stepper_name + "_s" + std::to_string(stages);
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
  namebuilder << rhs_name << "_" << stepper_tag;
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
