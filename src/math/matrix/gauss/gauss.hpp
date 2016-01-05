#ifndef ECHMET_MATRIX_GAUSSHPP
#define ECHMET_MATRIX_GAUSSHPP
// ============================================================================
// COMMENTS:

// $1) Pivoting needs abs function be defined on T
//     We import std::abs for integral, floating point and comlex types
//     For user-defined types, abs function must be defined
//     in the type's namespace so that ADL applies

//==============================================================================
// INCLUDES

#include <sstream>   // exception messages
#include <cstdlib>   // $1
#include <cmath>     // $1
// #include <complex>   // $1


#include "../core.hpp"

#include "gauss_wrap.hpp"
#include "gauss_switch.hpp"

#include "gauss.h"


//==============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//==============================================================================
// namespace IMPL

namespace impl {

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<class T, transtags TRANSPOSED>
inline void ADeterminant(Core<T, TRANSPOSED> const & core, T *Determinant)
throw()
{

	*Determinant = 0;

	for (

		typename Core<T, TRANSPOSED>::ConstDiagonal::iterator
			i     = core.GetDiagonal().begin(),
			i_end = core.GetDiagonal().end()

		;

		i != i_end

		;

		++i

	) *Determinant += *i;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<class T, transtags TRANSPOSED>
inline void
ARowCombination
(

	typename Core<T, TRANSPOSED>::Row & rCombine     ,
	typename Core<T, TRANSPOSED>::Row & rCombineWith ,
	msize_t                             index        ,
	T const &                           val

) throw()
{

	for (

		typename Core<T, TRANSPOSED>::Row::iterator
			row     = rCombine.begin()     + index ,
			row_end = rCombine.end()               ,
			combine = rCombineWith.begin() + index

		;

		row != row_end

		;

		++row, ++combine

	) *row += val * (*combine);

}

//==============================================================================
} // namespace impl

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags  TRANSPOSED>
inline msize_t GetPivotRowIndex(Core<T, TRANSPOSED> const & core, msize_t col)
throw()
{

	using std::abs;                                                         // $1

	typename Core<T, TRANSPOSED>::Transposed::ConstRow::iterator
		begin  = core.GetTransposed().Rows(col).begin(),
		result = begin + col
	;

	for
	(

		typename Core<T, TRANSPOSED>::Transposed::ConstRow::iterator
			i     = result + 1,
			i_end = core.GetTransposed().Rows(col).end()

		;

		i != i_end

		;

		++i

	)	if (abs(*i) > abs(*result) ) result = i;                              // $1

	return result - begin;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<

	typename        T              ,
	transtags       DT             ,
	typename        GSWT_ASSOCIATE ,
	typename        GSWT_PMUTATION ,
	gaussswitchtags GSST_SWITCH

>
void GaussElimination(

	Core<T, DT>                                        & core        ,
	typename GSWT_ASSOCIATE::template Wrapper<T>::ref    associate   ,
	typename GSWT_PMUTATION::template Wrapper<T>::ref    pmutation   ,
	T                                                  * determinant

)
{

	// typedefs

	typedef typename GSWT_ASSOCIATE::template Wrapper<T>::type a_type;
	typedef typename GSWT_PMUTATION::template Wrapper<T>::type p_type;

	// declarations

	typename a_type::Row       ra     = associate.Rows();
	typename p_type::Row       rp     = pmutation.Rows();
	typename Core<T, DT>::Row  rc     = core.Rows();
	typename Core<T, DT>::Row  rc_end = core.RowsEnd();
	--(rc_end.ptr);                                                          // 1

	// Gauss FOR loop

	for (

		msize_t i = 0;
		rc.ptr != rc_end.ptr;
		++(ra.ptr), ++(rp.ptr), ++(rc.ptr) /* ++i -> // !2 */

	) {

	  // pivoting

		T       a, aii;
		msize_t ipivot;

		ipivot = GetPivotRowIndex(core, i);

		rc.swap(core[ipivot]);
		ra.swap(associate.Rows(ipivot));
		rp.swap(pmutation.Rows(ipivot));

		aii = rc[i];

		// singularity check

		if ( aii == 0 ){

			std::stringstream s;

			s << "Gauss elimination of a singular matrix of size ["
				<< core.data.Height() << "][" << core.data.Width() << "]"
			;

			throw msingular_error(s.str());

		}

		// row combination

		// .. we go through the i-th column == i-th row of the transposed
		typename Core<T, DT>::Transposed::Row::iterator cl =
			core.GetTransposed().Rows(i).begin()
		;

		// .. declarations

		++i;                                                                  // !2

    typename a_type::Row       rra     = associate.Rows(i);
    typename Core<T, DT>::Row  rrc     = core.Rows(i);
    typename Core<T, DT>::Row  rrc_end = core.RowsEnd();
    cl += i;

    // .. row combination FOR loop

    for (

      msize_t j = i;
      rrc.ptr != rrc_end.ptr;
      ++j, ++(rra.ptr), ++(rrc.ptr), ++cl

		) {

			a = - *cl / aii;

			impl::ARowCombination<T, DT>(rrc, rc, i, a);
			rra.LinCombination(ra, a);

			// semting lower-triangular elements of *this

			impl::gauss_switch<GSST_SWITCH>::LTriang( *cl, a );

		} // row combination FOR

	} // Gauss FOR


	// determinant

	if (determinant) impl::ADeterminant(core, determinant);

}
// 1 ) Gauss elimination does not process the last row
//
// !2) from now on we would use $i+1$ many times so we increment $i$ here
//     instead when the next for loop comes
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<

	typename    T              ,
	transtags   DT             ,
	typename    GSWT_ASSOCIATE

>
void BackSubstitution (

	Core<T, DT>                                        & core      ,
	typename GSWT_ASSOCIATE::template Wrapper<T>::ref    associate

)
{

	// declarations

	typedef typename GSWT_ASSOCIATE::template Wrapper<T>::type a_type;

	typename a_type::Row       ra     = associate.RowsEnd();
	typename Core<T, DT>::Row  rc     = core.RowsEnd();
	typename Core<T, DT>::Row  rc_end = core.Rows();
	--(ra.ptr);                                                              // 1
	--(rc.ptr);                                                              // 1

	// back substitution FOR

	for (

		msize_t i = core.data.Height() - 1
		;
		rc.ptr != rc_end.ptr
		;
		--(ra.ptr), --(rc.ptr), --i

	) {

		// current row

		T aii = core[i][i];

		if ( aii == 0 ) {

			std::stringstream s;

			s << "Back substitution of a singular matrix of size ["
				<< core.data.Height() << "][" << core.data.Width() << "]"
			;

			throw msingular_error(s.str());

		} // if singular

		ra *= 1 / aii;

		// row combinations

		typename Core<T, DT>::Transposed::Row::iterator cl =
			core.GetTransposed().Rows(i).begin()
		;

		for (

			typename a_type::Row rra     = associate.Rows() ,
													 rra_end = associate.Rows(i)
			;
			rra.ptr != rra_end.ptr
			;
			++(rra.ptr), ++cl

		) rra.LinCombination( ra, -(*cl))	;

	} // for rows

	// finalize with the first row

	T a0 = core[0][0];

	if ( a0 == 0 ) {

		std::stringstream s;

		s << "Back substitution of a singular matrix of size ["
			<< core.data.Height() << "][" << core.data.Width() << "]"
		;

		throw msingular_error(s.str());

	} // if singular

	// finalize

	associate.Rows(0) *= 1 / a0;

}
// "one-past-the-end" -> the last
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<

	typename        T              ,
	transtags       DT             ,
	typename        GSWT_ASSOCIATE ,
	typename        GSWT_PMUTATION ,
	gaussswitchtags GSST_SWITCH

>
inline void GaussSolv (

	Core<T, DT>                                        & core        ,
	typename GSWT_ASSOCIATE::template Wrapper<T>::ref    associate   ,
	typename GSWT_PMUTATION::template Wrapper<T>::ref    pmutation   ,
	T                                                  * determinant

)
{

  GaussElimination < T, DT, GSWT_ASSOCIATE, GSWT_PMUTATION, GSST_SWITCH >
  (core, associate, pmutation, determinant);

	BackSubstitution<T, DT, GSWT_ASSOCIATE>	(core, associate);

}

//-----------------------------------------------------------------------------
// GaussSolver

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags  DT>
inline
Core<T, DT> &
GaussSolver<T, DT>::Inverse(Core<T, DT> & core)
{

  Core<T, trRows> acore(core, soft_copy());
  Core<T, trRows> identity(core.data.Height(), core.data.Height(), 1);

  GaussSolv<T, trRows, gswtReference<trRows>, gswtNull, gsstFast>
  (acore, identity);

	core.data.StealCopy(identity.data);

	return core;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags  DT> template<transtags  DTT>
inline
Core<T, DTT> &
GaussSolver<T, DT>::Solve(Core<T, DT> & core, Core<T, DTT> & associate)
{

  typedef Core<T, trRows>               acore_type;
  typedef Core<T, transtags(DT != DTT)> assoc_type;

  acore_type acore(core,      soft_copy());
  assoc_type assoc(associate, soft_copy());

  GaussSolv
  <

    T,
    trRows,
    gswtReference<transtags(DT != DTT)>,
    gswtNull,
    gsstFast

  >(acore, assoc);

  return associate;

}


//==============================================================================
}} // namespace echmet :: matrix

#endif // Header

