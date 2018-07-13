#ifndef ECHMET_MATRIX_DATA_ROWSPOINTERH
#define ECHMET_MATRIX_DATA_ROWSPOINTERH
//=============================================================================
// COMMENTS
//

// $1) -> $ C1 in fwdcl.h

// $2) -> $ C2 in fwdcl.h

// $3) -> $3 in rows_proxy.h

//=============================================================================
// INCLUDES

#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/and.hpp>
#include <boost/utility/enable_if.hpp>

#include "../../mystd/iterator_facade_ext.h"

#include "data_row.h"

//=============================================================================
// MACROS

// $1
#define ECHMET_MATRIX_ROWS_POINTER_FRIENDS                                     \
  template<typename TT, transtags_ DTT> friend class RowsPointer;              \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// RowsPointer

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
class RowsPointer : public boost::iterator_facade
<
  RowsPointer<T, TRANSPOSED>             ,
  DataRow<T, TRANSPOSED>                 ,
  boost::random_access_traversal_tag     ,
        DataRow<T, TRANSPOSED>
>
{
private:

  // Defs

  typedef boost::iterator_facade
  <
    RowsPointer<T, TRANSPOSED>           ,
    DataRow<T, TRANSPOSED>              ,
    boost::random_access_traversal_tag  ,
    DataRow<T, TRANSPOSED>
  > iterator_facade_type;

  // $3
  MYSTD_ITERATOR_FACADE_INTRODUCE(iterator_facade_type)

  void            increment   ()                          throw();
  void            decrement   ()                          throw();
  void            advance     (difference_type)           throw();

  template<typename TT>
  difference_type distance_to (RowsPointer<TT, TRANSPOSED> const &)
  const throw();

  template<typename TT>
  bool            equal       (RowsPointer<TT, TRANSPOSED> const &)
  const throw();

  reference       dereference () const throw();

  // <

  ECHMET_MATRIX_ROWS_POINTER_FRIENDS

  // Data

  RowsProxy<T, TRANSPOSED> proxy;

public:

  // Constructors

           RowsPointer()                                    throw();
  explicit RowsPointer(RowsProxy<T, TRANSPOSED>  const &)   throw();

  // $2
  RowsPointer(RowsPointer const &) throw();

  // $2
  template <typename TT>
  RowsPointer (

    RowsPointer <TT, TRANSPOSED> const & ,
    typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

  ) throw();

  // Assignment
  // $2

  RowsPointer & operator=(RowsPointer const &) throw();

  template <typename TT>
  typename boost::enable_if
        < boost::is_convertible<TT *, T *>, RowsPointer & >
  ::type
  operator=( RowsPointer <TT, TRANSPOSED> const & ) throw();

  // STD

  void swap(RowsPointer &) throw();

};


//-----------------------------------------------------------------------------
// > STD >

template<typename T, transtags_ DT>
void swap( RowsPointer<T, DT> &, RowsPointer<T, DT> & ) throw();

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header
