#ifndef ECHMET_MATRIX_GAUSS_SWITCHH
#define ECHMET_MATRIX_GAUSS_SWITCHH

//=============================================================================
// INCLUDES

#include "fwdcl.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<gaussswitchtags S> struct gauss_switch;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<>
struct gauss_switch<gsstFast> {

	template<typename T> static void LTriang(T &, const T &);

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<>
struct gauss_switch<gsstZero> {

	template<typename T> static void LTriang(T &, const T &);

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<>
struct gauss_switch<gsstLU> {

	template<typename T> static void LTriang(T &, const T &);

};

//=============================================================================
}}} // namespace echmet :: matrix :: impl


#endif // Header
