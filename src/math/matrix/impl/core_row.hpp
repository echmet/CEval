#ifndef ECHMET_MATRIX_CORE_ROWHPP
#define ECHMET_MATRIX_CORE_ROWHPP
//==============================================================================
// INCLUDES

#include "data_row.hpp"

#include "core_row.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//-----------------------------------------------------------------------------
// CORE ROW

//------------------------------------------------------------------------------
// .. Public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
CoreRow<T, TRANSPOSED>::CoreRow(DataRow<T, TRANSPOSED> const & arow) throw()
: DataRow<T, TRANSPOSED>(arow)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
CoreRow<T, TRANSPOSED>::CoreRow(CoreRow const & other) throw()
:
	DataRow<T, TRANSPOSED>(other)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT>
inline
CoreRow<T, TRANSPOSED>::CoreRow(
	CoreRow<TT, TRANSPOSED> const &	other,
	typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *
) throw()
:
	DataRow<T, TRANSPOSED>(other)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename Operation_Not_Allowed_For_Const_T<T, CoreRow<T, TRANSPOSED> &>::type
CoreRow<T, TRANSPOSED>::operator=( CoreRow const & other) throw()
{

	DataRow<T, TRANSPOSED>::operator=(other);

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
typename Operation_Not_Allowed_For_Const_T<T, CoreRow<T, TRANSPOSED> &>
::type
CoreRow<T, TRANSPOSED>::operator=(T const & t)
throw()
{

	DataRow<T, TRANSPOSED>::operator=(t);

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE
CoreRow<T, TRANSPOSED>::operator=(CoreRow<TR, DTR> const & other)
throw()
{

	DataRow<T, TRANSPOSED>::operator=(other);

  return *this;

}

//-----------------------------------------------------------------------------
// .. Arithmetics

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE
CoreRow<T, TRANSPOSED>::operator+=(CoreRow<TR, DTR> const & rval) throw()
{

	typename CoreRow<TR, DTR>::const_iterator ri = rval.begin();
	for (

    typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
                                              i_end = this->end()
    ;
    i != i_end
    ;
    ++i, ++ri

  ) *i += *ri;

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE
CoreRow<T, TRANSPOSED>::operator-=(CoreRow<TR, DTR> const & rval) throw()
{

	typename CoreRow<TR, DTR>::const_iterator ri = rval.begin();
	for (

		typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
																							i_end = this->end()
		;
		i != i_end
		;
		++i, ++ri

	) *i -= *ri;

	return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE
CoreRow<T, TRANSPOSED>::LinCombination(CoreRow<TR, DTR> const & rval, T const & val)
throw()
{

	typename CoreRow<TR, DTR>::const_iterator ri = rval.begin();
	for (

		typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
																							i_end = this->end()
		;
		i != i_end
		;
		++i, ++ri

	) *i += val * (*ri);

	return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE_T
CoreRow<T, TRANSPOSED>::operator+=(TR const & rval) throw()
{

	for (

    typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
                                              i_end = this->end()
    ;
    i != i_end
    ;
    ++i

  ) *i += rval;

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE_T
CoreRow<T, TRANSPOSED>::operator-=(TR const & rval) throw()
{

	for (

    typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
                                              i_end = this->end()
    ;
    i != i_end
    ;
    ++i

  ) *i -= rval;

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE_T
CoreRow<T, TRANSPOSED>::operator*=(TR const & rval) throw()
{

	for (

    typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
                                              i_end = this->end()
    ;
    i != i_end
    ;
    ++i

  ) *i *= rval;

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
ECHMET_MATRIX_CR_RETURN_SFINAE_T
CoreRow<T, TRANSPOSED>::operator/=(TR const & rval) throw()
{

	for (

    typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
                                              i_end = this->end()
    ;
    i != i_end
    ;
    ++i

  ) *i /= rval;

  return *this;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename Operation_Not_Allowed_For_Const_T<T, CoreRow<T, TRANSPOSED> &>::type
CoreRow<T, TRANSPOSED>::Negate() throw()
{

	for (

    typename CoreRow<T, TRANSPOSED>::iterator i     = this->begin(),
                                              i_end = this->end()
    ;
    i != i_end
    ;
    ++i

  ) *i = -(*i);

  return *this;

}

//-----------------------------------------------------------------------------
// Arithmetics (global)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
typename boost::remove_const<TL>::type
operator*(CoreRow<TL, DTL> const & lval, CoreRow<TR, DTR> const & rval)
{

	typename boost::remove_const<TL>::type result = 0;

	typename CoreRow<TR, DTR>::const_iterator   ri    = rval.begin();
	for (
		typename CoreRow<TL, DTL>::const_iterator i     = lval.begin(),
																							i_end = lval.end()
		;
		i != i_end
		;
		++i, ++ri
	)
		result += (*i) * (*ri)
	;

	return result;

}

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
swap( CoreRow<TL, DTL> const & a1 , CoreRow<DUMM, DTR> const & a2) throw()
{

#if MATRIX_DEFS_DEBUG_SWAP

		echmet::debug << "\nswap<CoreRow>" << echmet::endl;

#endif

	a1.swap(a2);

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header

