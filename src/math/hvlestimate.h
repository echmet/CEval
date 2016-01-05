#ifndef ECHMET_HVLESTIMATE_H
#define ECHMET_HVLESTIMATE_H

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

    a3d = -0.0024  * std::pow(T, 6)
          + 0.0627 * std::pow(T, 5)
          - 0.653  * std::pow(T, 4)
          + 3.4872 * std::pow(T, 3)
          - 9.8558 * std::pow(T, 2)
          + 21.765 * T
          - 14.662;

    K   = -0.0006  * std::pow(T, 6)
          + 0.0159 * std::pow(T, 5)
          - 0.1743 * std::pow(T, 4)
          + 1.01   * std::pow(T, 3)
          - 3.3048 * std::pow(T, 2)
          + 6.0382 * T - 3.5649;

    L = - 6.5048E-5 * std::pow(T, 6)
        + 1.6676E-3 * std::pow(T, 5)
        - 1.7078E-2 * std::pow(T, 4)
        + 8.7898E-2 * std::pow(T, 3)
        - 2.2774E-1 * std::pow(T, 2)
        + 2.0664E-1 * T
        + 3.7645E-1;

    a0   = area;
    a1   = t + (sign * K * fwhm);   // 1
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
