#ifndef MASS_SPRING_HPP
#define MASS_SPRING_HPP

#include <nonlinfunc.hpp>
#include <timestepper.hpp>

using namespace ASC_ode;

#include <vector.hpp>
using namespace nanoblas;

template <int D>
class Mass
{
public:
  double mass;
  Vec<D> pos;
  Vec<D> vel = 0.0;
  Vec<D> acc = 0.0;
};

template <int D>
class Fix
{
public:
  Vec<D> pos;
};

class Connector
{
public:
  enum CONTYPE
  {
    FIX = 1,
    MASS = 2
  };
  CONTYPE type;
  size_t nr;
};

std::ostream &operator<<(std::ostream &ost, const Connector &con)
{
  ost << "type = " << int(con.type) << ", nr = " << con.nr;
  return ost;
}

class Spring
{
public:
  double length;
  double stiffness;
  std::array<Connector, 2> connectors;
};

class DistanceConstraint
{
public:
  std::array<Connector, 2> connectors;
  double length;
};

template <int D>
class MassSpringSystem
{
  std::vector<Fix<D>> m_fixes;
  std::vector<Mass<D>> m_masses;
  std::vector<Spring> m_springs;
  std::vector<DistanceConstraint> m_constraints;
  Vec<D> m_gravity = 0.0;

public:
  void setGravity(Vec<D> gravity) { m_gravity = gravity; }
  Vec<D> getGravity() const { return m_gravity; }

  Connector addFix(Fix<D> p)
  {
    m_fixes.push_back(p);
    return {Connector::FIX, m_fixes.size() - 1};
  }

  Connector addMass(Mass<D> m)
  {
    m_masses.push_back(m);
    return {Connector::MASS, m_masses.size() - 1};
  }

  size_t addSpring(Spring s)
  {
    m_springs.push_back(s);
    return m_springs.size() - 1;
  }

  size_t addConstraint(DistanceConstraint c)
  {
    m_constraints.push_back(c);
    return m_constraints.size() - 1;
  }

  auto &fixes() { return m_fixes; }
  auto &masses() { return m_masses; }
  auto &springs() { return m_springs; }
  auto &constraints() { return m_constraints; }

  void getState(VectorView<> values, VectorView<> dvalues, VectorView<> ddvalues)
  {
    auto valmat = values.asMatrix(m_masses.size(), D);
    auto dvalmat = dvalues.asMatrix(m_masses.size(), D);
    auto ddvalmat = ddvalues.asMatrix(m_masses.size(), D);

    for (size_t i = 0; i < m_masses.size(); i++)
    {
      valmat.row(i) = m_masses[i].pos;
      dvalmat.row(i) = m_masses[i].vel;
      ddvalmat.row(i) = m_masses[i].acc;
    }
  }

  void setState(VectorView<> values, VectorView<> dvalues, VectorView<> ddvalues)
  {
    auto valmat = values.asMatrix(m_masses.size(), D);
    auto dvalmat = dvalues.asMatrix(m_masses.size(), D);
    auto ddvalmat = ddvalues.asMatrix(m_masses.size(), D);

    for (size_t i = 0; i < m_masses.size(); i++)
    {
      m_masses[i].pos = valmat.row(i);
      m_masses[i].vel = dvalmat.row(i);
      m_masses[i].acc = ddvalmat.row(i);
    }
  }
};

template <int D>
std::ostream &operator<<(std::ostream &ost, MassSpringSystem<D> &mss)
{
  ost << "fixes:" << std::endl;
  for (auto f : mss.fixes())
    ost << f.pos << std::endl;

  ost << "masses: " << std::endl;
  for (auto m : mss.masses())
    ost << "m = " << m.mass << ", pos = " << m.pos << std::endl;

  ost << "springs: " << std::endl;
  for (auto sp : mss.springs())
    ost << "length = " << sp.length << ", stiffness = " << sp.stiffness
        << ", C1 = " << sp.connectors[0] << ", C2 = " << sp.connectors[1] << std::endl;

  ost << "constraints: " << std::endl;
  for (auto c : mss.constraints())
    ost << "length = " << c.length << ", C1 = " << c.connectors[0] << ", C2 = " << c.connectors[1] << std::endl;
  return ost;
}

template <int D>
class MSS_Function : public NonlinearFunction
{
  MassSpringSystem<D> &mss;

public:
  MSS_Function(MassSpringSystem<D> &_mss)
      : mss(_mss) {}

  virtual size_t dimX() const override { return D * mss.masses().size() + mss.constraints().size(); }
  virtual size_t dimF() const override { return D * mss.masses().size() + mss.constraints().size(); }

  virtual void evaluate(VectorView<double> x, VectorView<double> f) const override
  {
    f = 0.0;

    size_t numMasses = mss.masses().size();
    size_t numConstraints = mss.constraints().size();

    auto xmat = x.range(0, D * numMasses).asMatrix(numMasses, D);
    auto fmat = f.range(0, D * numMasses).asMatrix(numMasses, D);

    for (size_t i = 0; i < numMasses; i++)
      fmat.row(i) = mss.masses()[i].mass * mss.getGravity();

    for (auto spring : mss.springs())
    {
      auto [c1, c2] = spring.connectors;
      Vec<D> p1, p2;
      if (c1.type == Connector::FIX)
        p1 = mss.fixes()[c1.nr].pos;
      else
        p1 = xmat.row(c1.nr);
      if (c2.type == Connector::FIX)
        p2 = mss.fixes()[c2.nr].pos;
      else
        p2 = xmat.row(c2.nr);

      double force = spring.stiffness * (norm(p1 - p2) - spring.length);
      Vec<D> dir12 = 1.0 / norm(p1 - p2) * (p2 - p1);
      if (c1.type == Connector::MASS)
        fmat.row(c1.nr) += force * dir12;
      if (c2.type == Connector::MASS)
        fmat.row(c2.nr) -= force * dir12;
    }

    for (size_t i = 0; i < numMasses; i++)
      fmat.row(i) *= 1.0 / mss.masses()[i].mass;

    for (size_t i = 0; i < numConstraints; i++)
    {
      double lambda = x(D * numMasses + i);
      auto &con = mss.constraints()[i];
      auto [c1, c2] = con.connectors;

      Vec<D> p1, p2;
      if (c1.type == Connector::FIX)
        p1 = mss.fixes()[c1.nr].pos;
      else
        p1 = xmat.row(c1.nr);
      if (c2.type == Connector::FIX)
        p2 = mss.fixes()[c2.nr].pos;
      else
        p2 = xmat.row(c2.nr);

      Vec<D> diff = p1 - p2;
      if (c1.type == Connector::MASS)
        fmat.row(c1.nr) += (2 * lambda) * diff;
      if (c2.type == Connector::MASS)
        fmat.row(c2.nr) -= (2 * lambda) * diff;

      f(D * numMasses + i) = dot(p1 - p2, p1 - p2) - con.length * con.length;
    }
  }

  virtual void evaluateDeriv(VectorView<double> x, MatrixView<double> df) const override
  {
    size_t numMasses = mss.masses().size();
    size_t numConstraints = mss.constraints().size();
    const size_t n = D * numMasses + numConstraints;

    for (size_t i = 0; i < df.rows(); i++)
      for (size_t j = 0; j < df.cols(); j++)
        df(i, j) = 0.0;

    auto xmat = x.range(0, D * numMasses).asMatrix(numMasses, D);

    for (auto spring : mss.springs())
    {
      auto [c1, c2] = spring.connectors;

      Vec<D> p1 = (c1.type == Connector::FIX) ? mss.fixes()[c1.nr].pos : xmat.row(c1.nr);
      Vec<D> p2 = (c2.type == Connector::FIX) ? mss.fixes()[c2.nr].pos : xmat.row(c2.nr);

      Vec<D> diff = p1 - p2;
      double L = norm(diff);
      if (L == 0.0)
        continue;
      Vec<D> dir = diff;
      for (size_t k = 0; k < D; k++)
        dir(k) /= L;

      // Spring stiffness matrix (outer product)
      for (int a = 0; a < D; a++)
        for (int b = 0; b < D; b++)
        {
          double val = spring.stiffness * ((a == b ? 1.0 : 0.0) - dir(a) * dir(b));
          if (c1.type == Connector::MASS && c2.type == Connector::MASS)
          {
            df(D * c1.nr + a, D * c1.nr + b) += val;
            df(D * c1.nr + a, D * c2.nr + b) -= val;
            df(D * c2.nr + a, D * c1.nr + b) -= val;
            df(D * c2.nr + a, D * c2.nr + b) += val;
          }
          else if (c1.type == Connector::MASS && c2.type == Connector::FIX)
            df(D * c1.nr + a, D * c1.nr + b) += val;
          else if (c1.type == Connector::FIX && c2.type == Connector::MASS)
            df(D * c2.nr + a, D * c2.nr + b) += val;
        }
    }

    // Constraints contributions
    for (size_t i = 0; i < numConstraints; i++)
    {
      auto &con = mss.constraints()[i];
      auto [c1, c2] = con.connectors;

      Vec<D> p1 = (c1.type == Connector::FIX) ? mss.fixes()[c1.nr].pos : xmat.row(c1.nr);
      Vec<D> p2 = (c2.type == Connector::FIX) ? mss.fixes()[c2.nr].pos : xmat.row(c2.nr);

      Vec<D> diff = p1 - p2;

      for (int a = 0; a < D; a++)
      {
        if (c1.type == Connector::MASS)
        {
          df(D * c1.nr + a, D * numMasses + i) += 2.0 * diff(a); // derivative wrt lambda
          df(D * numMasses + i, D * c1.nr + a) += 2.0 * diff(a); // derivative wrt position
        }
        if (c2.type == Connector::MASS)
        {
          df(D * c2.nr + a, D * numMasses + i) -= 2.0 * diff(a);
          df(D * numMasses + i, D * c2.nr + a) -= 2.0 * diff(a);
        }
      }

      // Derivative of constraint w.r.t lambda is zero (already handled in force positions)
    }

    // Divide forces by mass for acceleration part
    for (size_t i = 0; i < numMasses; i++)
    {
      double m = mss.masses()[i].mass;
      for (size_t a = 0; a < D; a++)
      {
        for (size_t j = 0; j < D * numMasses + numConstraints; j++)
          df(D * i + a, j) /= m;
      }
    }
  }
};

#endif
