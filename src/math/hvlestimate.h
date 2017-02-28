#ifndef ECHMET_HVLESTIMATE_H
#define ECHMET_HVLESTIMATE_H

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

} // namespace HVLCore
} // namespace echmet

#endif //ECHMET_HVLESTIMATE_H
