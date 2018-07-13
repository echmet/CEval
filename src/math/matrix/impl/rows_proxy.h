#ifndef ECHMET_MATRIX_ROWS_PROXYH
#define ECHMET_MATRIX_ROWS_PROXYH
//=============================================================================
// COMMENTS
//

// $1) -> fwdcl.h

// $3) RowPointer operator== (technically the equal method) compares only
//     positions of two pointers assuming that IsSharedWith evaluates to true,
//     i.e. both pointers belong to the same data.
//
//     It is usually of no use to compare equality between pointers
//     pointing to different data (effectively memory pools).
//     Should it be of any use, use IsSame method.
//
//     -> reference from data_rowspointer.h

// $4) provate ctors : rows_proxy not intended for public use
//
// $4b) ... but def ctor and cctor must be available
//          for the boost iterator facade mechanism
//          (making boost::iterator_core_access is not enough)
//          iterators are expected to be default- and copy- constructible

//=============================================================================
// INCLUDES

#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>

#include "../../mystd/iterator_facade_ext.h"

#include "cells_pointer.h"

//=============================================================================
// MACROS

// $1
#define ECHMET_MATRIX_ROWS_PROXY_FRIENDS                                \
  template<typename, transtags_ >                                       \
  friend class RowsProxy;                                               \
                                                                        \
  template<typename, transtags_ , typename, transtags_ >                \
	friend class RowsProxyIsSameHelper;                                   \
																																				\
	friend class RowsProxyMethods<T, TRANSPOSED>;                         \
																																				\
	template<typename, transtags_ , typename, transtags_>                 \
	friend class DataRowMethods;                                          \
																																				\
	friend class RowsPointer<T, TRANSPOSED>;                              \
	friend class DataRow<T, TRANSPOSED>;                                  \
																																				\
	template<typename, transtags>                                         \
	friend class Data;                                                    \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// RowsProxyIsSameHelper

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
class RowsProxyIsSameHelper {
private:

  friend class RowsProxy<TL, DTL>;
  friend class RowsProxy<TR, DTR>;

  static bool IsSame ( RowsProxy<TL, DTL> const &, RowsProxy<TR, DTR> const & )
  throw();

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DT, typename TR>
class RowsProxyIsSameHelper<TL, DT, TR, DT> {
private:

	friend class RowsProxy<TL, DT>;
	friend class RowsProxy<TR, DT>;

	static bool IsSame( RowsProxy<TL, DT> const &, RowsProxy<TR, DT> const & )
	throw();

};

//-----------------------------------------------------------------------------
// RowsProxyMethods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
class RowsProxyMethods<T, trNormal> {
private:

  friend class RowsProxy<T, trNormal>;

  static
  CellsPointer<T, trNormal>
  Cell(RowsProxy<T, trNormal> const &, msize_t j) throw();

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
class RowsProxyMethods<T, trTransposed> {
private:

  friend class RowsProxy<T, trTransposed>;

  static
  CellsPointer<T, trTransposed>
  Cell(RowsProxy<T, trTransposed> const &, msize_t j) throw();

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
class RowsProxyMethods<T, trDiagonal> {
private:

  friend class RowsProxy<T, trDiagonal>;

  static
  CellsPointer<T, trDiagonal>
  Cell(RowsProxy<T, trDiagonal> const &, msize_t j) throw();

};

//-----------------------------------------------------------------------------
// CLASS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
class RowsProxy : public boost::iterator_facade
<
  RowsProxy<T, TRANSPOSED>             ,
  CellsPointer<T, TRANSPOSED>          ,
  boost::random_access_traversal_tag   ,
  CellsPointer<T, TRANSPOSED>
>
{
private:

  // Defs

  typedef boost::iterator_facade
  <

		RowsProxy<T, TRANSPOSED>            ,
		CellsPointer<T, TRANSPOSED>         ,
		boost::random_access_traversal_tag  ,
		CellsPointer<T, TRANSPOSED>

	> iterator_facade_type;

  // $2
  MYSTD_ITERATOR_FACADE_INTRODUCE(iterator_facade_type)

  void            increment   ()                          throw();
  void            decrement   ()                          throw();
  void            advance     (difference_type)           throw();

  template<typename TT>
  difference_type distance_to (RowsProxy<TT, TRANSPOSED> const &)
  const throw();

  template<typename TT>
  bool            equal       (RowsProxy<TT, TRANSPOSED> const &)
  const throw();

  reference       dereference () const throw();

  // <

  ECHMET_MATRIX_ROWS_PROXY_FRIENDS

  // Helpers

  template<typename TR, transtags_ DTR> struct mpl_GetIsSameHelper
  { typedef RowsProxyIsSameHelper<T, TRANSPOSED, TR, DTR> type; };         // 1

  // Data

  T **       rows;
  msize_t    width;
  mptrdiff_t i;

	// Constructors
	// $4

        //RowsProxy(T**, msize_t, mptrdiff_t) throw();

public:
        RowsProxy(T**, msize_t, mptrdiff_t) throw(); /* <- Originally private */

	// Constructors
	// $4b

	RowsProxy()                         throw();

	// $2
	RowsProxy(RowsProxy const &) throw();

	// $2
	template <typename TT>
	RowsProxy (

		RowsProxy <TT, TRANSPOSED> const & ,
		typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

	) throw();

	// Assignment
  // $2

  RowsProxy & operator=(RowsProxy const &) throw();

  template <typename TT>
  typename boost::enable_if
	< boost::is_convertible<TT *, T *>, RowsProxy & >
  ::type
  operator=( RowsProxy <TT, TRANSPOSED> const & ) throw();

  // Methods

  msize_t Width() const throw();

	template<typename TR, transtags_ DTR>
	bool IsSame      (RowsProxy<TR, DTR> const &) const throw();            // $3

	template<typename TR, transtags_ DT>
	bool IsSharedWith(RowsProxy<TR, DT> const &)  const throw();

	reference Cell(msize_t j) const throw();

  // STD

	void swap(RowsProxy &) throw();

};
// 1) RAD XE3 workaround
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
void swap(RowsProxy<T, TRANSPOSED> & lhs, RowsProxy<T, TRANSPOSED> & rhs);


//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header
