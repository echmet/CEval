#ifndef ECHMET_MATRIX_CORE_ROWSPOINTERHPP
#define ECHMET_MATRIX_CORE_ROWSPOINTERHPP
//==============================================================================
// COMMENTS

//==============================================================================
// INCLUDES

#include "data_rowspointer.hpp"
#include "core_row.hpp"

#include "core_rowspointer.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// CoreRowsPointer

//-----------------------------------------------------------------------------
// .. iterator facade

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void CoreRowsPointer<T, TRANSPOSED>::increment() throw()
{

	++ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void CoreRowsPointer<T, TRANSPOSED>::decrement() throw()
{

	--ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void CoreRowsPointer<T, TRANSPOSED>::advance(difference_type n) throw()
{

	ptr += n;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT>
inline
typename CoreRowsPointer<T, TRANSPOSED>::difference_type
CoreRowsPointer<T, TRANSPOSED>::distance_to(const CoreRowsPointer<TT, TRANSPOSED> & other)
const throw()
{

	return other.ptr - ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT>
inline
bool CoreRowsPointer<T, TRANSPOSED>::equal
(const CoreRowsPointer<TT, TRANSPOSED> & other)
const throw()
{

	return ptr == other.ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename CoreRowsPointer<T, TRANSPOSED>::reference
CoreRowsPointer<T, TRANSPOSED>::dereference()
const throw()
{

	return reference(*ptr);

}

//-----------------------------------------------------------------------------
// .. public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
CoreRowsPointer<T, TRANSPOSED>::CoreRowsPointer()
throw()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
CoreRowsPointer<T, TRANSPOSED>::CoreRowsPointer(RowsPointer<T, TRANSPOSED>  const & aptr)
throw()
: ptr(aptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
CoreRowsPointer<T, TRANSPOSED>::CoreRowsPointer(CoreRowsPointer const & other)
throw()
: ptr(other.ptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template <typename TT>
inline
CoreRowsPointer<T, TRANSPOSED>::CoreRowsPointer (

	CoreRowsPointer <TT, TRANSPOSED> const & other                             ,
	typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *

) throw()
: ptr(other.ptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
CoreRowsPointer<T, TRANSPOSED> &
CoreRowsPointer<T, TRANSPOSED>::operator=(const CoreRowsPointer & other)
throw()
{

	ptr = other.ptr;

	return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template <typename TT>
inline
typename boost::enable_if
<
	boost::is_convertible<TT *, T *> ,
	CoreRowsPointer<T, TRANSPOSED> &
>
::type
CoreRowsPointer<T, TRANSPOSED>::operator=(const CoreRowsPointer<TT, TRANSPOSED> & other)
throw()
{

	ptr = other.ptr;

	return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void
CoreRowsPointer<T, TRANSPOSED>::swap(CoreRowsPointer & other)
throw()
{

	ptr.swap(other.ptr);

}

//-----------------------------------------------------------------------------
// Std

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ DT>
inline
void swap( CoreRowsPointer<T, DT> & p1, CoreRowsPointer<T, DT> & p2 )
throw()
{

	p1.swap(p2);

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header

