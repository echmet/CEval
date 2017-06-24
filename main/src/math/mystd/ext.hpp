#ifndef MYSTD_EXT_HPP
#define MYSTD_EXT_HPP

#include <cmath>

#include "ext.h"

//===========================================================================
//---------------------------------------------------------------------------

namespace myStd {

inline double Exp(double x) {	return x > MYSTD_MAX_EXP_ARGUMENT ? MYSTD_Infinity : std::exp(x); }

inline double Ln(double x) {	return std::log(x); }

// todo: using boost rounding functions
inline double Round(double r) { return (r > 0. ? std::floor(r + 0.5) : std::ceil(r - 0.5)); }

inline template<class FROM, class TO> TO rebuild_cast(FROM a) { return TO(a); }

} // namespace

//---------------------------------------------------------------------------
#endif // Header
