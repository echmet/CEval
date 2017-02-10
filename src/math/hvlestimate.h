#ifndef ECHMET_HVLESTIMATE_H
#define ECHMET_HVLESTIMATE_H

#include <cmath>

namespace echmet {

namespace HVLCore {

class Coefficients
{
public:
    double a0;
    double a1;
    double a2;
    double a3d;

    Coefficients();
    Coefficients(double a0, double a1, double a2, double a3d);
    Coefficients(double area, double t, double fwhm, double tusp, int);

    Coefficients(Coefficients const &) = default;
    Coefficients & operator=(Coefficients const &) = default;

    Coefficients Calculate(double area, double t, double fwhm, double tusp);

};

//---------------------------------------------------------------------------
inline Coefficients::Coefficients() : a0(1), a1(0), a2(1), a3d(0) {}

//---------------------------------------------------------------------------
inline Coefficients::Coefficients(double a0, double a1, double a2, double a3d)
: a0(a0), a1(a1), a2(a2), a3d(a3d)
{}

//---------------------------------------------------------------------------
inline Coefficients::Coefficients(double area, double t, double fwhm, double tusp, int)
: Coefficients()
{

    Calculate(area, t, fwhm, tusp);

}

//---------------------------------------------------------------------------
inline Coefficients Coefficients::Calculate
(double area, double t, double fwhm, double tusp)
{

    double T = tusp;
    double sign = 1;

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

    a0   = area;
    a1   = t - (sign * K * fwhm);   // 1
    a2   = L * fwhm;
    a3d *= sign;

    return *this;
}

// 1 : v clanku je "-", protoze ta fce generuje "-K".
//     - je to tak generovano uz v excel sesitu.
//     - fce "K" v clanku ma byt "-K", je tam chyba.
//--------------------------------------------------------------------------

} // namespace HVLCore
} // namespace echmet

#endif //ECHMET_HVLESTIMATE_H
