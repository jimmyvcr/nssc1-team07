#ifndef AUTODIFF_HPP
#define AUTODIFF_HPP

#include <cstddef> 
#include <ostream> 
#include <cmath>   
#include <array>  


namespace ASC_ode
{

  template <size_t N, typename T = double>
  class Variable 
  {
    private:
      T m_val;
    public:
      Variable (T v) : m_val(v) {}
      T value() const { return m_val; }
  };

  template <typename T = double>
  auto derivative (T v, size_t /*index*/) { return T(0); } 


  template <size_t N, typename T = double>
  class AutoDiff
  {
  private:
    T m_val;
    std::array<T, N> m_deriv;
  public: 
    AutoDiff () : m_val(0), m_deriv{} {}
    AutoDiff (T v) : m_val(v), m_deriv{} 
    {
      for (size_t i = 0; i < N; i++)
        m_deriv[i] = derivative(v, i);
    }
    
    template <size_t I>
    AutoDiff (Variable<I, T> var) : m_val(var.value()), m_deriv{} 
    {
      m_deriv[I] = 1.0;
    }

    T value() const { return m_val; }
    std::array<T, N>& deriv() { return m_deriv; }
    const std::array<T, N>& deriv() const { return m_deriv; }
  };


  template <size_t N, typename T = double>
  auto derivative (AutoDiff<N, T> v, size_t index) 
  {
    return v.deriv()[index];
  }



  template <size_t N, typename T>
  std::ostream & operator<< (std::ostream& os, const AutoDiff<N, T>& ad)
  {
    os << "Value: " << ad.value() << ", Deriv: [";
    for (size_t i = 0; i < N; i++)
    {
      os << ad.deriv()[i];
      if (i < N - 1) os << ", ";
    }
    os << "]";
    return os;
  }

  template <size_t N, typename T = double>
  AutoDiff<N, T> operator+ (const AutoDiff<N, T>& a, const AutoDiff<N, T>& b)
  {
     AutoDiff<N, T> result(a.value() + b.value());
     for (size_t i = 0; i < N; i++)
        result.deriv()[i] = a.deriv()[i] + b.deriv()[i];
       return result;
   }

   template <size_t N, typename T = double>
   auto operator+ (T a, const AutoDiff<N, T>& b) { return AutoDiff<N, T>(a) + b; }


   template <size_t N, typename T = double>
   AutoDiff<N, T> operator* (const AutoDiff<N, T>& a, const AutoDiff<N, T>& b)
   {
       AutoDiff<N, T> result(a.value() * b.value());
       for (size_t i = 0; i < N; i++)
          result.deriv()[i] = a.deriv()[i] * b.value() + a.value() * b.deriv()[i];
       return result;
   }

   template <size_t N, typename T = double>
   AutoDiff<N, T> operator* (T a, const AutoDiff<N, T> &b) 
   {
        AutoDiff<N, T> result(a * b.value());
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = a * b.deriv()[i];
        return result;
    }

   template <size_t N, typename T = double>
   AutoDiff<N, T> operator/ (const AutoDiff<N, T> &a, const AutoDiff<N, T> &b)
   {
       AutoDiff<N, T> result(a.value() / b.value());
       for (size_t i = 0; i < N; i++)
          result.deriv()[i] = (a.deriv()[i] * b.value() - a.value() * b.deriv()[i]) / (b.value() * b.value());
       return result;
   }

   template <size_t N, typename T = double>
   AutoDiff<N, T> operator- (const AutoDiff<N, T> &a, const AutoDiff<N, T> &b)
   {
       AutoDiff<N, T> result(a.value() - b.value());
       for (size_t i = 0; i < N; i++)
          result.deriv()[i] = a.deriv()[i] - b.deriv()[i];
       return result;
   }

   template <size_t N, typename T = double>
   AutoDiff<N, T> operator- (const AutoDiff<N, T> &a)
   {
       AutoDiff<N, T> result(-a.value());
       for (size_t i = 0; i < N; i++)
          result.deriv()[i] = -a.deriv()[i];
       return result;
   }
   
   template <size_t N, typename T = double>
   bool operator== (const AutoDiff<N, T> &a, const AutoDiff<N, T> &b) 
   {
        bool value = a.value() == b.value();
        return value && (a.deriv() == b.deriv());
   }

   using std::sin;
   using std::cos;

   template <size_t N, typename T = double>
   AutoDiff<N, T> sin(const AutoDiff<N, T> &a)
   {
       AutoDiff<N, T> result(sin(a.value()));
       for (size_t i = 0; i < N; i++)
           result.deriv()[i] = cos(a.value()) * a.deriv()[i];
       return result;
   }

    template <size_t N, typename T = double>
    AutoDiff<N, T> cos(const AutoDiff<N, T> &a)
    {
        AutoDiff<N, T> result(cos(a.value()));
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = -sin(a.value()) * a.deriv()[i];
        return result;
    }

    template <size_t N, typename T = double>
    AutoDiff<N,T> tan(const AutoDiff<N,T>& a) {
        AutoDiff<N,T> result(std::tan(a.value()));

        T sec2 = 1.0 / std::cos(a.value());
        sec2 = sec2 * sec2;
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = sec2 * a.deriv()[i];
        return result;
    }

    template <size_t N, typename T = double>
    AutoDiff<N, T> exp(const AutoDiff<N, T> &a)
    {
        AutoDiff<N, T> result(exp(a.value()));
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = exp(a.value()) * a.deriv()[i];
        return result;
    }

    template <size_t N, typename T = double>
    AutoDiff<N, T> log(const AutoDiff<N, T> &a)
    {
        AutoDiff<N, T> result(log(a.value()));
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = 1/a.value() * a.deriv()[i];
        return result;
    }

    template <size_t N, typename T = double>
    AutoDiff<N, T> pow(const AutoDiff<N, T> &a, T exp)
    {
        AutoDiff<N, T> result(std::pow(a.value(), exp));
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = exp * std::pow(a.value(), exp - 1) * a.deriv()[i];
        return result;
    }

    template <size_t N, typename T = double>
    AutoDiff<N, T> pow(T a, const AutoDiff<N, T> &exp)
    {
        AutoDiff<N, T> result(std::pow(a, exp.value()));
        for (size_t i = 0; i < N; i++)
            result.deriv()[i] = std::pow(a, exp.value())* std::log(a) * exp.deriv()[i];
        return result;
    }

    template <size_t N, typename T = double>
    AutoDiff<N, T> sqrt(const AutoDiff<N, T>& a)
    {
        return pow(a, T(0.5));
    }


} // namespace ASC_ode

#endif
