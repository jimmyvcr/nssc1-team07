#ifndef TIMERSTEPPER_HPP
#define TIMERSTEPPER_HPP

#include <functional>
#include <exception>

#include "Newton.hpp"


namespace ASC_ode {
class TimeStepper {
 protected:
    std::shared_ptr<NonlinearFunction> m_rhs;
 public:
    TimeStepper(std::shared_ptr<NonlinearFunction> rhs) : m_rhs(rhs) {}
    virtual ~TimeStepper() = default;
    virtual void DoStep(double tau, VectorView<double> y) = 0;
};

class ExplicitEuler : public TimeStepper {
  Vector<> m_vecf;
 public:
    ExplicitEuler(std::shared_ptr<NonlinearFunction> rhs)
    : TimeStepper(rhs), m_vecf(rhs->dimF()) {}
    void DoStep(double tau, VectorView<double> y) override {
      this->m_rhs->evaluate(y, m_vecf);
      y += tau * m_vecf;
    }
};

class ImprovedEuler : public TimeStepper {
  Vector<> m_vecf;
  Vector<> m_vecf_til;
 public:
  ImprovedEuler(std::shared_ptr<NonlinearFunction> rhs)
  : TimeStepper(rhs), m_vecf(rhs->dimF()), m_vecf_til(rhs->dimF()) {}
  void DoStep(double tau, VectorView<double> y) override {
    this->m_rhs->evaluate(y, m_vecf);
    Vector<> ytil = y;
    ytil += (tau * 0.5) * m_vecf;
    this->m_rhs->evaluate(ytil, m_vecf_til);
    y += tau * m_vecf_til;
  }
};

class ImplicitEuler : public TimeStepper {
  std::shared_ptr<NonlinearFunction> m_equ;
  std::shared_ptr<Parameter> m_tau;
  std::shared_ptr<ConstantFunction> m_yold;
 public:
    ImplicitEuler(std::shared_ptr<NonlinearFunction> rhs)
    : TimeStepper(rhs), m_tau(std::make_shared<Parameter>(0.0)) {
      m_yold = std::make_shared<ConstantFunction>(rhs->dimX());
      auto ynew = std::make_shared<IdentityFunction>(rhs->dimX());
      m_equ = ynew - m_yold - m_tau * m_rhs;
    }

  void DoStep(double tau, VectorView<double> y) override {
    m_yold->set(y);
    m_tau->set(tau);
    NewtonSolver(m_equ, y);
  }
};

class CrankNicolson : public TimeStepper
{
  std::shared_ptr<NonlinearFunction> m_equ;
  std::shared_ptr<Parameter> m_tau;
  std::shared_ptr<ConstantFunction> m_yold;
public:
  CrankNicolson(std::shared_ptr<NonlinearFunction> rhs) 
    : TimeStepper(rhs), m_tau(std::make_shared<Parameter>(0.0)) 
  {
    m_yold = std::make_shared<ConstantFunction>(rhs->dimX());
    auto ynew = std::make_shared<IdentityFunction>(rhs->dimX());
    m_equ = ynew - m_yold - m_tau * (0.5 * (Compose(m_rhs, m_yold) + m_rhs));
  }

  void DoStep(double tau, VectorView<double> y) override
  {
    m_yold->set(y);
    m_tau->set(tau);
    NewtonSolver(m_equ, y);
  }
};
}  // namespace ASC_ode


#endif  // NSSC1_TEAM07_SRC_TIMESTEPPER_HPP_
