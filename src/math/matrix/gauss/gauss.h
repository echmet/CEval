#ifndef ECHMET_MATRIX_GAUSSH
#define ECHMET_MATRIX_GAUSSH
//==============================================================================
// COMMENTS
//

//==============================================================================
// INCLUDES

#include "../core.h"

#include "gauss_wrap.h"
#include "gauss_switch.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// namespace IMPL

namespace impl {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void ADeterminant(Core<T, TRANSPOSED> const &, T *) throw();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void ARowCombination
(

	typename Core<T, TRANSPOSED>::Row & rCombine     ,
	typename Core<T, TRANSPOSED>::Row & rCombineWith ,
	msize_t                             index        ,
	T const &                           val

) throw();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void ARowSwap(Core<T, TRANSPOSED> &, msize_t, msize_t) throw();

//=============================================================================
} // namespace impl


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
msize_t GetPivotRowIndex(Core<T, TRANSPOSED> const &, msize_t) throw();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<

	typename        T              ,
	transtags       DT             ,
	typename        GSWT_ASSOCIATE ,
	typename        GSWT_PMUTATION ,
	gaussswitchtags GSST_SWITCH

>
void GaussElimination (

	Core<T, DT>                                        &         , // core
	typename GSWT_ASSOCIATE::template Wrapper<T>::ref    = NULL  , // associate
	typename GSWT_PMUTATION::template Wrapper<T>::ref    = NULL  , // permutation
	T                                                  * = NULL    // determinant

);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<

	typename   T              ,
	transtags  DT             ,
	typename   GSWT_ASSOCIATE

>
void BackSubstitution (

	Core<T, DT>                                        &        , // core
	typename GSWT_ASSOCIATE::template Wrapper<T>::ref    = NULL   // associate

);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<

	typename        T              ,
	transtags       DT             ,
	typename        GSWT_ASSOCIATE ,
	typename        GSWT_PMUTATION ,
	gaussswitchtags GSST_SWITCH

>
void GaussSolv (

	Core<T, DT>                                        &        , // core
	typename GSWT_ASSOCIATE::template Wrapper<T>::ref    = NULL , // associate
	typename GSWT_PMUTATION::template Wrapper<T>::ref    = NULL , // permutation
	T                                                  * = NULL   // determinant

);

//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags DT>
struct GaussSolver {

	static Core<T, DT> & Inverse(Core<T, DT> &);

	template<transtags DTT>
	static Core<T, DTT> & Solve(Core<T, DT> &, Core<T, DTT> &);

};

//==============================================================================
}} // namespace echmet :: matrix

#endif // Header

