#ifndef ECHMET_MATRIX_DEFSH
#define ECHMET_MATRIX_DEFSH
//=============================================================================
// COMMENTS

// --- Comments $C1) - $C3) are common to several headers in the Matrix lib ---

// $ C1) We make CLASS<TT> friend so as to be able to do CLASS<T> -> CLASS<const T>
//       conversion.
//       - while boost::enable_if idiom disables CLASS<const T>  -> CLASS<T>
//         conversion
//       Since there is the second TRANSPOSE template parameter and
//       Partial specialization of nested classes is not allowed,
//       we have to make full templatization CLASS<TT, DTT> friend.

// $ C2) Templatized cctor does not override default cctor so that both
//      (non)-templatized cctors must exist, the templatized because of $C1
//      and the non-templatized as to override the default.
//      -> hmtp://stackoverflow.com/questions/24758427/compiler-destructs-an-optimized-out-object-never-created
//
//      The same applies to operator=.

// $ C3) SFINAE (enable_if idiom) does not work if used in a template class
//       and beased on the template's parameter itself
//       -> hmtp://stackoverflow.com/questions/11248173/enable-if-seems-to-work-outside-a-class-but-not-inside

// $ C4) MYSTD_ITERATOR_FACADE_INTERFACE macro is not applicable in the Matrix
//       classes as they are multi-parametrized
//       -> MyStd\iterator_facade_ext.h

// -> referenced from impl\*.*

//=============================================================================
// INCLUDES

#include <stdexcept>

#include "gauss/fwdcl.h" // includes basic_dcl.h

// ============================================================================
// SETTINGS

#define ECHMET_MATRIX_DEFS_DEBUG_ALLOC false
#define ECHMET_MATRIX_DEFS_DEBUG_SWAP  false

// ============================================================================
// DEFINES

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define ECHMET_MATRIX_HEADER                                                   \
	template                                                                     \
  <                                                                            \
    typename  T,                                                               \
    transtags TRN,	                                                           \
    invtags   INV,                                                             \
    datatags  DTG,                                                             \
    template<typename, transtags> class SLV                                    \
  >

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define ECHMET_MATRIX_DCL  < T, TRN, INV, DTG, SLV >

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// CODE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Data & Core

template<typename = double, transtags = trRows> class Data;
template<typename = double, transtags = trRows> class Core;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Matrix

template
<

	typename                              = double      ,
	transtags                             = trRows       ,
	invtags                               = ivNormal    ,
	datatags                              = dtUnique    ,
	template<typename, transtags> class Q = GaussSolver

> class Matrix_;

typedef Matrix_<> Matrix;


//-----------------------------------------------------------------------------
// Exceptions

class msize_error;
class msquare_error;
class msingular_error;

#define DEFINE_ERROR(ERROR, PARENT)     \
	class ERROR : public PARENT {         \
	public:                               \
		ERROR(std::string s) : PARENT(s) {} \
	};

DEFINE_ERROR(matrix_error,    std::logic_error)
DEFINE_ERROR(msize_error,     matrix_error)
DEFINE_ERROR(msquare_error,   matrix_error)
DEFINE_ERROR(msingular_error, matrix_error)

//=============================================================================
// NAMESPACE impl

namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// Transposition specific data access

enum transtags_ { trNormal = false, trTransposed = true, trDiagonal };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename, transtags_>  class CellsPointer;
template<typename T>            class CellsPointer<T, trNormal>;
template<typename T>            class CellsPointer<T, trTransposed>;
template<typename T>            class CellsPointer<T, trDiagonal>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename, transtags_>  class RowsProxy;

template<typename, transtags_>  class RowsProxyMethods;
template<typename T>            class RowsProxyMethods<T, trNormal>;
template<typename T>            class RowsProxyMethods<T, trTransposed>;
template<typename T>            class RowsProxyMethods<T, trDiagonal>;

template<typename, transtags_, typename, transtags_>
class RowsProxyIsSameHelper;

template<typename TL, transtags_ DTL, typename TR>
class RowsProxyIsSameHelper<TL, DTL, TR, DTL>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename, transtags_> class RowsPointer;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename, transtags_>                        class DataRow;
template<typename, transtags_, typename, transtags_>  class DataRowMethods;

template<typename TL, typename TR>
class DataRowMethods<TL, trNormal, TR, trNormal>;

//-----------------------------------------------------------------------------
// Matrix

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DataMethods

template<typename, transtags>  struct DataMethods;
template<typename T>           struct DataMethods<T, trRows>;
template<typename T>           struct DataMethods<T, trColumns>;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Base

ECHMET_MATRIX_HEADER           class Base;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Policy

template<datatags>             class unique_policy;
template<datatags,  datatags>  class copy_policy;
template<transtags, transtags> class trans_policy;
template<invtags,   invtags>   class inv_policy;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Arithmetics

template<int>              class selfTmultiply;

//-----------------------------------------------------------------------------
// Common SFINAE helper
// $ 3

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, typename TYPE>
struct Operation_Not_Allowed_For_Const_T
{

	typedef TYPE type;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, typename TYPE>
struct Operation_Not_Allowed_For_Const_T<const T, TYPE>
{

	template<typename TT> class disabled {};
	typedef disabled<const T> type;

};

//=============================================================================
} // namespace impl

//=============================================================================
}} // namespace echmet :: matrix

#endif // Header

