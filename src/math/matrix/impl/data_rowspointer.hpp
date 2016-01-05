#ifndef ECHMET_MATRIX_DATA_ROWSPOINTERHPP
#define ECHMET_MATRIX_DATA_ROWSPOINTERHPP
//==============================================================================
// COMMENTS

//==============================================================================
// INCLUDES

#include "data_row.hpp"

#include "data_rowspointer.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// RowsPointer

//-----------------------------------------------------------------------------
// .. iterator facade

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsPointer<T, TRANSPOSED>::increment() throw()
{

  ++proxy;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsPointer<T, TRANSPOSED>::decrement() throw()
{

  --proxy;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsPointer<T, TRANSPOSED>::advance(difference_type n) throw()
{

  proxy += n;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT>
inline
typename RowsPointer<T, TRANSPOSED>::difference_type
RowsPointer<T, TRANSPOSED>::distance_to(const RowsPointer<TT, TRANSPOSED> & other)
const throw()
{

  return other.proxy - proxy;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT>
inline
bool RowsPointer<T, TRANSPOSED>::equal
(const RowsPointer<TT, TRANSPOSED> & other)
const throw()
{

	return proxy == other.proxy;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename RowsPointer<T, TRANSPOSED>::reference
RowsPointer<T, TRANSPOSED>::dereference()
const throw()
{

  return reference(proxy);

}

//-----------------------------------------------------------------------------
// .. public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsPointer<T, TRANSPOSED>::RowsPointer()
throw()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsPointer<T, TRANSPOSED>::RowsPointer(RowsProxy<T, TRANSPOSED>  const & aproxy)
throw()
: proxy(aproxy)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsPointer<T, TRANSPOSED>::RowsPointer(RowsPointer const & other)
throw()
: proxy(other.proxy)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template <typename TT>
inline
RowsPointer<T, TRANSPOSED>::RowsPointer (

  RowsPointer <TT, TRANSPOSED> const & other                             ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *

) throw()
: proxy(other.proxy)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsPointer<T, TRANSPOSED> &
RowsPointer<T, TRANSPOSED>::operator=(const RowsPointer & other)
throw()
{

  proxy = other.proxy;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template <typename TT>
inline
typename boost::enable_if
<
  boost::is_convertible<TT *, T *> ,
  RowsPointer<T, TRANSPOSED> &
>
::type
RowsPointer<T, TRANSPOSED>::operator=(const RowsPointer<TT, TRANSPOSED> & other)
throw()
{

  proxy = other.proxy;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsPointer<T, TRANSPOSED>::swap(RowsPointer & other)
throw()
{

  proxy.swap(other.proxy);

}

//-----------------------------------------------------------------------------
// Std

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ DT>
inline
void swap
(
  RowsPointer<T, DT> & p1,
  RowsPointer<T, DT> & p2
)
throw()
{

  p1.swap(p2);

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header

