#ifndef ECHMET_MATRIX_GAUSS_SWITCHHPP
#define ECHMET_MATRIX_GAUSS_SWITCHHPP

//=============================================================================
// INCLUDES

#include "gauss_switch.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline void gauss_switch<gsstFast>::LTriang(T &, const T &)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline void gauss_switch<gsstZero>::LTriang(T & data, const T &)
{

	data = 0;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline void gauss_switch<gsstLU>::LTriang(T & data, const T & a)
{

	data = -a;

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header
