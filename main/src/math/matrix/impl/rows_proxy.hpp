#ifndef ECHMET_MATRIX_ROWS_PROXYHPP
#define ECHMET_MATRIX_ROWS_PROXYHPP
//==============================================================================
// COMMENTS

//==============================================================================
// INCLUDES

#include "cells_pointer.hpp"

#include "rows_proxy.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// RowsProxyIsSameHelper

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
inline
bool RowsProxyIsSameHelper<TL, DTL, TR, DTR>::IsSame
(

  RowsProxy<TL, DTL> const &,
  RowsProxy<TR, DTR> const &

) throw()
{

	return false;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DT, typename TR>
inline
bool RowsProxyIsSameHelper<TL, DT, TR, DT>::IsSame
(

	RowsProxy<TL, DT> const & p1,
	RowsProxy<TR, DT> const & p2

) throw()
{

	return p1.IsSharedWith(p2) && p1 == p2;

}

//-----------------------------------------------------------------------------
// RowsProxyMethods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trNormal>
RowsProxyMethods<T, trNormal>::Cell(
  RowsProxy<T, trNormal> const & p,
  msize_t                        j
)
throw()
{

  return CellsPointer<T, trNormal>(p.rows[p.i], j);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trTransposed>
RowsProxyMethods<T, trTransposed>::Cell(
  RowsProxy<T, trTransposed> const & p,
  msize_t                            j
)
throw()
{

  return CellsPointer<T, trTransposed>(p.rows, p.i, j);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trDiagonal>
RowsProxyMethods<T, trDiagonal>::Cell(
  RowsProxy<T, trDiagonal> const & p,
  msize_t                          j
)
throw()
{

  return CellsPointer<T, trDiagonal>(p.rows, j);

}

//-----------------------------------------------------------------------------
// CLASS

//-----------------------------------------------------------------------------
// .. iterator facade

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsProxy<T, TRANSPOSED>::increment() throw()
{

  ++i;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsProxy<T, TRANSPOSED>::decrement() throw()
{

  --i;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
void RowsProxy<T, TRANSPOSED>::advance(difference_type n) throw()
{

  i += n;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
template<typename TT>
inline
typename RowsProxy<T, TRANSPOSED>::difference_type
RowsProxy<T, TRANSPOSED>::distance_to
(RowsProxy<TT, TRANSPOSED> const & other)
const throw()
{

  return other.i - i;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
template<typename TT>
inline
bool RowsProxy<T, TRANSPOSED>::equal
(RowsProxy<TT, TRANSPOSED> const & other)
const throw()
{

  return other.i == i;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template<typename T, transtags_ TRANSPOSED>
inline
typename RowsProxy<T, TRANSPOSED>::reference
RowsProxy<T, TRANSPOSED>::dereference()
const throw()
{

  class mystd_rows_proxy_dereference_disabled {
  private:
    mystd_rows_proxy_dereference_disabled ();
  } mystd_rows_proxy_dereference_disabled_;

  return Cell(*this, 0);

}

//-----------------------------------------------------------------------------
// .. public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsProxy<T, TRANSPOSED>::RowsProxy()
throw()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsProxy<T, TRANSPOSED>::RowsProxy(T ** arows, msize_t awidth, mptrdiff_t ai)
throw()
: rows(arows), width(awidth), i(ai)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsProxy<T, TRANSPOSED>::RowsProxy(RowsProxy const & other)
throw()
: rows(other.rows), width(other.width), i(other.i)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template <typename TT>
inline
RowsProxy<T, TRANSPOSED>::RowsProxy (

  RowsProxy <TT, TRANSPOSED> const & other                             ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *

) throw()
: rows((T**)other.rows), width(other.width), i(other.i)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
RowsProxy<T, TRANSPOSED> &
RowsProxy<T, TRANSPOSED>::operator=(const RowsProxy & other)
throw()
{

  rows  = other.rows;
  width = other.width;
  i     = other.i;

  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template <typename TT>
inline
typename boost::enable_if
<
  boost::is_convertible<TT *, T *> ,
  RowsProxy<T, TRANSPOSED> &
>
::type
RowsProxy<T, TRANSPOSED>::operator=(const RowsProxy<TT, TRANSPOSED> & other)
throw()
{

  rows  = (T**)other.rows;
  width = other.width;
  i     = other.i;

  return *this;

}

//-----------------------------------------------------------------------------
// .. Methods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
msize_t RowsProxy<T, TRANSPOSED>::Width() const throw()
{

  return width;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL> template<typename TR, transtags_ DTR>
inline
bool RowsProxy<TL, DTL>::IsSame(RowsProxy<TR, DTR> const & other)
const throw()
{

  typedef typename mpl_GetIsSameHelper<TR, DTR>::type dsh;
  return dsh::IsSame(*this, other);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT, transtags_ DT>
inline
bool RowsProxy<T, TRANSPOSED>::IsSharedWith(RowsProxy<TT, DT> const & other)
const throw()
{

  return rows == other.rows;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename RowsProxy<T, TRANSPOSED>::reference
RowsProxy<T, TRANSPOSED>::Cell(msize_t j)
const throw()
{

  return RowsProxyMethods<T, TRANSPOSED>::Cell(*this, j);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_
 TRANSPOSED>
inline
void RowsProxy<T, TRANSPOSED>::swap(RowsProxy & other)
throw()
{

  std::swap(rows,  other.rows);
  std::swap(width, other.width);
  std::swap(i,     other.i);

}

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
void swap(RowsProxy<T, TRANSPOSED> & lhs, RowsProxy<T, TRANSPOSED> & rhs)
{

	lhs.swap(rhs);

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header

