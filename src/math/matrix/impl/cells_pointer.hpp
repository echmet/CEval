#ifndef ECHMET_MATRIX_CELLS_POINTERHPP
#define ECHMET_MATRIX_CELLS_POINTERHPP
//==============================================================================
// COMMENTS

// $ 1) We do not care about i in equal and distance as these operations
//      are only reasonable for iterators of the same container (row)

//==============================================================================
// INCLUDES

#include "../fwdcl.h"

#include "cells_pointer.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// trNormal

//-----------------------------------------------------------------------------
// .. iterator facade

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trNormal>::increment() throw()
{

  ++ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trNormal>::decrement() throw()
{

  --ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trNormal>::advance(difference_type n) throw()
{

  ptr += n;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template<typename TT>
inline
typename CellsPointer<T, trNormal>::difference_type
CellsPointer<T, trNormal>::distance_to(CellsPointer<TT, trNormal> const & other)
const throw()
{

  return other.ptr - ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template<typename TT>
inline
bool
CellsPointer<T, trNormal>::equal(CellsPointer<TT, trNormal> const & other)
const throw()
{

  return other.ptr == ptr;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
typename CellsPointer<T, trNormal>::reference
CellsPointer<T, trNormal>::dereference()
const throw()
{

  return *ptr;

}

//-----------------------------------------------------------------------------
// trNormal / public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trNormal>::CellsPointer()
throw()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trNormal>::CellsPointer(T * aptr, mptrdiff_t aj)
throw()
: ptr(aptr + aj)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trNormal>::CellsPointer(const CellsPointer & other)
throw()
: ptr(other.ptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template <typename TT>
inline
CellsPointer<T, trNormal>::CellsPointer (

  const CellsPointer <TT, trNormal> & other ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *

) throw()
: ptr(other.ptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trNormal> &
CellsPointer<T, trNormal>::operator=(const CellsPointer & other)
throw()
{

  ptr = other.ptr;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template <typename TT>
inline
typename boost::enable_if
<
  boost::is_convertible<TT *, T *> ,
  CellsPointer<T, trNormal> &
>
::type
CellsPointer<T, trNormal>::operator=(const CellsPointer<TT, trNormal> & other)
throw()
{

  ptr = other.ptr;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trNormal>::swap(CellsPointer & other)
throw()
{

  std::swap(ptr, other.ptr);

}

//-----------------------------------------------------------------------------
// trTransposed

//-----------------------------------------------------------------------------
// .. iterator facade

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trTransposed>::increment() throw()
{

  ++rows;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trTransposed>::decrement() throw()
{

  --rows;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trTransposed>::advance(difference_type n) throw()
{

  rows += n;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template<typename TT>
inline
typename CellsPointer<T, trTransposed>::difference_type
CellsPointer<T, trTransposed>::distance_to(const CellsPointer<TT, trTransposed> & other)
const throw()
{

  return const_cast<T**>(other.rows) - const_cast<T**>(rows);             // $1

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template<typename TT>
inline
bool
CellsPointer<T, trTransposed>::equal(const CellsPointer<TT, trTransposed> & other)
const throw()
{

  return const_cast<T**>(other.rows) == const_cast<T**>(rows);            // $1

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
typename CellsPointer<T, trTransposed>::reference
CellsPointer<T, trTransposed>::dereference()
const throw()
{

  return (*rows)[i];

}


//-----------------------------------------------------------------------------
// .. public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trTransposed>::CellsPointer()
throw()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trTransposed>::CellsPointer
(
  T ** arows,
  mptrdiff_t ai,
  mptrdiff_t aj
)
throw()
: rows(arows + aj), i(ai)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trTransposed>::CellsPointer(const CellsPointer & other)
throw()
: rows(other.rows), i(other.i)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template <typename TT>
inline
CellsPointer<T, trTransposed>::CellsPointer (

  const CellsPointer <TT, trTransposed> & other ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *

) throw()
: rows((T**)other.rows), i(other.i)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trTransposed> &
CellsPointer<T, trTransposed>::operator=(const CellsPointer & other)
throw()
{

  rows = other.rows;
  i    = other.i;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template <typename TT>
inline
typename boost::enable_if
<
  boost::is_convertible<TT *, T *> ,
  CellsPointer<T, trTransposed> &
>
::type
CellsPointer<T, trTransposed>::operator=(const CellsPointer<TT, trTransposed> & other)
throw()
{

  rows = (T**)other.rows;
  i    = other.i;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trTransposed>::swap(CellsPointer & other)
throw()
{

  std::swap(rows, other.rows);
  std::swap(i,    other.i);

}

//-----------------------------------------------------------------------------
// trDiagonal

//-----------------------------------------------------------------------------
// .. iterator facade

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trDiagonal>::increment() throw()
{

  ++i;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trDiagonal>::decrement() throw()
{

  --i;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trDiagonal>::advance(difference_type n) throw()
{

  i += n;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template<typename TT>
inline
typename CellsPointer<T, trDiagonal>::difference_type
CellsPointer<T, trDiagonal>::distance_to(const CellsPointer<TT, trDiagonal> & other)
const throw()
{

  return other.i - i;                                                     // $1

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template<typename TT>
inline
bool
CellsPointer<T, trDiagonal>::equal(const CellsPointer<TT, trDiagonal> & other)
const throw()
{

  return other.i == i;                                                    // $1

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
typename CellsPointer<T, trDiagonal>::reference
CellsPointer<T, trDiagonal>::dereference()
const throw()
{

  return rows[i][i];

}


//-----------------------------------------------------------------------------
// .. public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trDiagonal>::CellsPointer()
throw()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trDiagonal>::CellsPointer( T ** arows, mptrdiff_t ai)
throw()
: rows(arows), i(ai)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trDiagonal>::CellsPointer(const CellsPointer & other)
throw()
: rows(other.rows), i(other.i)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template <typename TT>
inline
CellsPointer<T, trDiagonal>::CellsPointer (

  const CellsPointer <TT, trDiagonal> & other ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *

) throw()
: rows((T**)other.rows), i(other.i)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
CellsPointer<T, trDiagonal> &
CellsPointer<T, trDiagonal>::operator=(const CellsPointer & other)
throw()
{

  rows = other.rows;
  i    = other.i;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> template <typename TT>
inline
typename boost::enable_if
<
  boost::is_convertible<TT *, T *> ,
  CellsPointer<T, trDiagonal> &
>
::type
CellsPointer<T, trDiagonal>::operator=(const CellsPointer<TT, trDiagonal> & other)
throw()
{

  rows = (T**)other.rows;
  i    = other.i;
  return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline
void CellsPointer<T, trDiagonal>::swap(CellsPointer & other)
throw()
{

  std::swap(rows, other.rows);
  std::swap(i,    other.i);

}

//-----------------------------------------------------------------------------
// Std

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags DT>
inline
void swap
(
  CellsPointer<T, DT> & p1,
  CellsPointer<T, DT> & p2
)
throw()
{

  p1.swap(p2);

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header



