#ifndef ECHMET_MATRIX_CORE_ROWH
#define ECHMET_MATRIX_CORE_ROWH
//=============================================================================
// COMMENTS

// $1 ) -> $ C1 in fwdcl.h

// $2 ) -> $ C2 in fwdcl.h

// $3 ) We need TR because we want to call things like Row<double, ...> += int(10)
//      boost::is_same<TR, TR> is a dummy test. We only need to test
//      boost::is_same<T, typename boost::remove_const<T>::type >
//      but we need to use TR as well because of -> $ C3 in fwdcl.h

// $4) -> $10 in data_row.h

//=============================================================================
// INCLUDES

#include <boost/mpl/and.hpp>

#include "data_row.h"

//=============================================================================
// MACROS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $1
#define ECHMET_MATRIX_CORE_ROW_FRIENDS                                  \
	template<typename, transtags_>          friend class CoreRow;         \

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define ECHMET_MATRIX_CR_RETURN_SFINAE                                  \
	template<typename TR, transtags_ DTR>                                 \
	inline                                                                \
	typename boost::enable_if                                             \
	<                                                                     \
    boost::is_same<T, typename boost::remove_const<TR>::type >,         \
		CoreRow<T, TRANSPOSED>                                              \
	>::type &                                                             \

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $3
#define ECHMET_MATRIX_CR_RETURN_SFINAE_T                                \
	template<typename TR>                                                 \
	typename boost::enable_if                                             \
	<                                                                     \
    boost::mpl::and_                                                    \
    <                                                                   \
  		boost::is_same<T, typename boost::remove_const<T>::type >,        \
  		boost::is_same<TR, TR>                                            \
    >,                                                                  \
    CoreRow<T, TRANSPOSED>                                              \
	>::type &                                                             \


//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// CLASS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
class CoreRow :  public DataRow<T, TRANSPOSED>
{
private:

	ECHMET_MATRIX_CORE_ROW_FRIENDS

public:

	// Ctors

	explicit CoreRow( DataRow<T, TRANSPOSED> const & ) throw();

	// $2
	CoreRow(CoreRow const &) throw();

	// $2
	template<typename TT>
	CoreRow(

		CoreRow<TT, TRANSPOSED> const &
		,
		typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

	) throw();

	// Assignment
	// $2

	typename Operation_Not_Allowed_For_Const_T<T, CoreRow<T, TRANSPOSED> &>::type
	operator=(CoreRow const &) throw();

	ECHMET_MATRIX_CR_RETURN_SFINAE operator=(CoreRow<TR, DTR> const &) throw();

	typename Operation_Not_Allowed_For_Const_T<T, CoreRow<T, TRANSPOSED> &>::type
	operator=(T const &)                throw();

	// STD

	// inherited swap, $4

	// Arithmetics

	ECHMET_MATRIX_CR_RETURN_SFINAE   operator+= (CoreRow<TR, DTR> const &)               throw();
	ECHMET_MATRIX_CR_RETURN_SFINAE   operator-= (CoreRow<TR, DTR> const &)               throw();
	ECHMET_MATRIX_CR_RETURN_SFINAE   LinCombination(CoreRow<TR, DTR> const &, T const &) throw();

	ECHMET_MATRIX_CR_RETURN_SFINAE_T operator+=(TR const &) throw();
	ECHMET_MATRIX_CR_RETURN_SFINAE_T operator-=(TR const &) throw();
	ECHMET_MATRIX_CR_RETURN_SFINAE_T operator*=(TR const &) throw();
	ECHMET_MATRIX_CR_RETURN_SFINAE_T operator/=(TR const &) throw();

	typename Operation_Not_Allowed_For_Const_T<T, CoreRow<T, TRANSPOSED> &>::type
	Negate() throw();

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
// Arithmetics

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
typename boost::remove_const<TL>::type
operator*(CoreRow<TL, DTL> const &, CoreRow<TR, DTR> const &);

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $4
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
swap( CoreRow<TL, DTL> const &, CoreRow<DUMM, DTR> const &) throw();

//=============================================================================
}}}  // namespace echmet :: matrix :: impl

#endif // Header



