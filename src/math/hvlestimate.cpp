#include "hvlestimate.h"
#include <cmath>

namespace echmet {

namespace HVLCore {

Coefficients::Coefficients() :
    a0(1), a1(0), a2(1), a3d(0)
{
}

Coefficients::Coefficients(const double a0, const double a1, const double a2, const double a3d) :
    a0(a0), a1(a1), a2(a2), a3d(a3d)
{
}

Coefficients::Coefficients(const Coefficients &other) :
    a0(other.a0), a1(other.a1), a2(other.a2), a3d(other.a3d)
{
}

Coefficients & Coefficients::operator=(const Coefficients &other)
{
    const_cast<double &>(a0) = other.a0;
    const_cast<double &>(a1) = other.a1;
    const_cast<double &>(a2) = other.a2;
    const_cast<double &>(a3d) = other.a3d;

    return *this;
}

Coefficients Coefficients::Calculate(const double area, const double t, const double fwhm, const double tusp)
{

    double T = tusp;
    double sign = 1;
    double a3d;

    if (T < 1.) { T = T / (2. * T - 1); sign = -1.;  }

    double K, L;

    /*
       a3d estimation function

       Běhehrádek's model:

       y = a * (x - b)^c

     */
    {
        static const double a = 6.75238;
        static const double b = 0.78126;
        static const double c = 1.11211;

        a3d = a * std::pow(T - b, c);
    }

    /*
      K function for a2 parameter estimate

      Rational fractional function:

      y = (b + c * x) / (1 + a * x)

      */
    {
      static const double a = -7.98009;
      static const double b = -16.16297;
      static const double c = 15.97496;

      K = (b + c * T) / (1 + a * T);
    }

    /*
       K function for a1 parameter estimate

       Rational fractional function:

       y = a * (d + x^b) / (e + x^c)

       */
    {
      static const double a = 0.55439;
      static const double b = 1.68759;
      static const double c = 2.24811;
      static const double d = 0.22110;
      static const double e = 0.59183;

      L = a * (d + std::pow(T, b)) / (e + std::pow(T, c));
    }

    const double a0   = area;
    const double a1   = t - (sign * K * fwhm);   // 1
    const double a2   = L * fwhm;
    a3d *= sign;

    return Coefficients(a0, a1, a2, a3d);
}

// 1 : v clanku je "-", protoze ta fce generuje "-K".
//     - je to tak generovano uz v excel sesitu.
//     - fce "K" v clanku ma byt "-K", je tam chyba.
//--------------------------------------------------------------------------

} // namespace HVLCore
} // namespace echmet
