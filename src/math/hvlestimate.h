#ifndef ECHMET_HVLESTIMATE_H
#define ECHMET_HVLESTIMATE_H

#include <cmath>

namespace echmet {

namespace HVLCore {

class Coefficients
{
public:
    const double a0;
    const double a1;
    const double a2;
    const double a3d;

    Coefficients();
    Coefficients(const double a0, const double a1, const double a2, const double a3d);
    Coefficients & operator=(Coefficients const &);

    static Coefficients Calculate(const double area, const double t, const double fwhm, const double tusp);

};

inline
int guessMPFRPrecision(const double in_a3)
{
  const double a3 = std::abs(in_a3);
  const double linear_term = 17.0 + a3 / 2.9;
  const double exp_term = std::exp(0.0125 * (a3 - 38.0));

  return static_cast<int>(std::floor(linear_term + exp_term + 0.5));
}

} // namespace HVLCore
} // namespace echmet

#endif //ECHMET_HVLESTIMATE_H
