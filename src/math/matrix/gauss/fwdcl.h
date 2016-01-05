#ifndef ECHMET_MATRIX_GAUSS_FWDCLH
#define ECHMET_MATRIX_GAUSS_FWDCLH
//==============================================================================
// COMMENTS
//

//==============================================================================
// INCLUDES

#include "../basic_dcl.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

enum gaussswitchtags { gsstFast, gsstZero, gsstLU };

struct gswtNull;
template<transtags>           struct gswtReference;

template<typename, transtags> struct GaussSolver;


//==============================================================================
}} // namespace echmet :: matrix

#endif // Header

