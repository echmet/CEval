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

    a3d = - 7.0314510E-05 * std::pow(T, 6)
          + 0.0029964061 * std::pow(T, 5)
          - 0.0504087123 * std::pow(T, 4)
          + 0.4192587314 * std::pow(T, 3)
          - 1.659194832 * std::pow(T, 2)
          + 11.0520416259 * T
          - 9.4041906377;

    K   = + 1.2613657E-06 * std::pow(T, 8)
          - 7.2546260E-05 * std::pow(T, 7)
          + 0.0017711503 * std::pow(T, 6)
          - 0.0239360421 * std::pow(T, 5)
          + 0.1958219401 * std::pow(T, 4)
          - 0.9965250412 * std::pow(T, 3)
          + 3.119561001 * std::pow(T, 2)
          - 5.7078188237 * T
          + 3.3847455286;

    L = - 1.3436376E-07 * std::pow(T, 8)
        + 7.5277456E-06 * std::pow(T, 7)
        - 0.0001773468 * std::pow(T, 6)
        + 0.0022779991 * std::pow(T, 5)
        - 0.0172494574 * std::pow(T, 4)
        + 0.0770821460 * std::pow(T, 3)
        - 0.1859409997 * std::pow(T, 2)
        + 0.1457839822 * T
        + 0.4070344262;


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
