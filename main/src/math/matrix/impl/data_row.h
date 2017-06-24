#ifndef ECHMET_MATRIX_DATA_ROWH
#define ECHMET_MATRIX_DATA_ROWH
//=============================================================================
// COMMENTS

// $1) -> $ C1) in fwdcl.h

// $2) -> $ C2) in fwdcl.h

// $3) Secures both that T is not const and TT is (const) T
//     We do not allow TT -> T conversion for any TT != T

// $4) We require DUMM be T, but we need DUMM for SFINAE here.
//     -> $C3) in data_defs.h
//
//     Both T of $this$ and the swap operand must not be const.
//     DUMM (not T) must be in the operand for the automated template type deduction
//     - this is different to $3, where only T must not be const but TR can be.
//       thus <TR> stays in the function body declaration in $3,
//       and is required (is not a dummy param in $3).
//       On the other hand, <T> stays in the function body declaration in here,
//       and DUMM is only a dummy parameter for the SFINAE

// $5) <DELETED>

// $6) Transposition conversion DTR -> DT is not allowed for cpy construction
//     but is allowed for operator=
//     -> $7

// $7) Row operations, namely operator= and swap,
//     work with the data pointed to by the row, rather than the row itself.
//     For example operator=(other) does not mean that $this$ points to $other$
//     but that the data pointed to by $this$ become physical copy of $other$.
//
//     -> DataRow.docx
//     -> ConstPolicy.docx
//     -> referenced from $6
//     -> referenced from $10

// $8) swap ADL does not work properly in RAD XE3. Works fine with gcc.
//     Even it is not possible to make swap friend with enable_if idiom
//     in RAD XE3. So we swap as follows:
//     - swap DataRow member function is for not making global swap friend
//     - global namespace swapis for ADL
//     - -> http://stackoverflow.com/questions/11562/how-to-overload-stdswap
//
//     ! Notice that DataRow is reference-like structure. Swapping in
//       the std-like style { DataRow tmp = r2; r2 = r1; r1 = tmp; }
//       does not work since r2 = r1 physically rewrites tmp...
//
//       So calling algorithms not aware of safe DataRow swapping
//       is potentially dangerous!
//
//       -> DataRow.docx
//
//     ! It's all but problematic. Call a.swap(b) if needed.
//       Do not use the "using swap" idiom.
//       Not gonna solve it now.

// $9) -> $C2 in fwdcl.h

// $10) we can swap const Rows of non-const T
//      ( although const Row returns const iterator etc. )
//      This is a compromise between strict const-correctness
//      and the ability to swap two rows of non-const matrices
//      as swap(m1[i], m2[j]); *)
//
//      *) here m#[#] are Row temporaries and thus cannot be
//         assigned to non-const reference...
//
//      -> referenced from core_row.h

// $11) yet another RAD XE3 workaround:
//      DataRowMethods could be in principle of
//      template <T, transtags_, transtags_> with template Assign method
//      and without any need for the ldata_type and rdata_type typedefs
//      (but RAD XE3 cant handle it)

//=============================================================================
// INCLUDES

#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/remove_const.hpp>

#include "../../mystd/rac_facade.h"

#include "cells_pointer.h"
#include "rows_proxy.h"

//=============================================================================
// MACROS

// $ 1
#define ECHMET_MATRIX_DATA_ROW_FRIENDS                                         \
                                                                               \
	template<typename, transtags_ >                                              \
	friend class DataRow;                                                        \
                                                                               \
	template<typename, transtags_ , typename, transtags_ >                       \
	friend class DataRowMethods;                                                 \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// DataMethods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $11
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
class DataRowMethods {
private:

	template<typename TT, transtags_ DTT> friend class DataRow;

	typedef DataRow<TL, DTL> ldata_type;
	typedef DataRow<TR, DTR> rdata_type;

	static void Assign( ldata_type &, rdata_type const & )
	throw();

  // $10
	static void Swap( ldata_type const &, rdata_type const &)
	throw();

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $11
template<typename TL, typename TR>
class DataRowMethods<TL, trNormal, TR, trNormal> {
private:

	template<typename TT, transtags_ DTT> friend class DataRow;

	typedef DataRow<TL, trNormal> ldata_type;
	typedef DataRow<TR, trNormal> rdata_type;

	static void Assign( ldata_type &, rdata_type const & )
	throw();

  // $10
	static void Swap( ldata_type const &, rdata_type const &)
	throw();

};

//-----------------------------------------------------------------------------
// CLASS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ DT>
class DataRow
: public

  myStd::mkrac_facade
  <

		DataRow<T, DT>
		,
		myStd::container_iterators_list<
			CellsPointer<T, DT>,
			CellsPointer<const T, DT>
		>

	>::type
{

private:

	ECHMET_MATRIX_DATA_ROW_FRIENDS

	// Helpers

	template<typename TR, transtags_ DTR>
	DataRow & AAssign( const DataRow<TR, DTR> & ) throw();

	// rac_facade CRTP interface

	typedef typename ::myStd::mkrac_facade<

		DataRow<T, DT>
		,
		myStd::container_iterators_list<
			CellsPointer<T, DT>,
			CellsPointer<const T, DT>
		>

	>::type rac_facade_type;

	MYSTD_RAC_FACADE_INTRODUCE(rac_facade_type)
	MYSTD_RAC_FACADE_INTERFACE

	// <

public:

	RowsProxy<T, DT> ptr;                                                   // $7

	// Ctors

	explicit DataRow( RowsProxy<T, DT> const & ) throw();

	DataRow(DataRow const &) throw();                                       // $2

	template<typename TT>
	DataRow(

		DataRow<TT, DT> const &                                       // $2, $6, $7
		,
		typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

	) throw();

	// Assignment
	// $2, $3, $7, $9

  typename Operation_Not_Allowed_For_Const_T<T, DataRow<T, DT> &>::type   // $9
	operator=(DataRow const &) throw();

	template<typename TR, transtags_ DTR>
	typename boost::enable_if
	<
		boost::is_same<T, typename boost::remove_const<TR>::type >,
		DataRow &
	>::type
	operator=(DataRow<TR, DTR> const &) throw();                            // $6

	typename Operation_Not_Allowed_For_Const_T<T, DataRow<T, DT> &>::type   // $9
	operator=(T const &) throw();

	// Other

	msize_t Width() const throw();

	// STD

	// $4, $10
	template<typename DUMM, transtags_ DTR>
	typename boost::enable_if
	<
		boost::mpl::and_
		<
			boost::is_same<T, typename boost::remove_const<DUMM>::type> ,
			boost::is_same<T, DUMM>
		>
		, void
	>
	::type
	swap(DataRow<DUMM, DTR> const &) const throw();

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//=============================================================================

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $8, $10
template<typename TL, transtags_ DTL, typename DUMM, transtags_ DTR>
typename boost::enable_if
<
	boost::mpl::and_
	<
		boost::is_same<TL, typename boost::remove_const<DUMM>::type> ,
		boost::is_same<TL, DUMM>
	>
	, void
>
::type
swap( DataRow<TL, DTL> const &, DataRow<DUMM, DTR> const &) throw();

//=============================================================================
}}}  // namespace echmet :: matrix :: impl

//=============================================================================
#endif // Header

