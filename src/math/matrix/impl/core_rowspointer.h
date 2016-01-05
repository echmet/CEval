#ifndef ECHMET_MATRIX_CORE_ROWSPOINTERH
#define ECHMET_MATRIX_CORE_ROWSPOINTERH
//=============================================================================
// COMMENTS

// $1) -> $ C1 in fwdcl.h

// $2) -> $ C4 in fwdcl.h

//=============================================================================
// INCLUDES

#include "data_rowspointer.h"
#include "core_row.h"

//=============================================================================
// MACROS

// $1
#define ECHMET_MATRIX_COREROWS_POINTER_FRIENDS                               \
	template<typename TT, transtags_ DTT> friend class CoreRowsPointer;        \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// CoreRowsPointer

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
class CoreRowsPointer
: public

    boost::iterator_facade
    <

      CoreRowsPointer<T, TRANSPOSED>         ,
      CoreRow<T, TRANSPOSED>                 ,
      boost::random_access_traversal_tag     ,
			CoreRow<T, TRANSPOSED>

		>
{
private:

	// Defs

	typedef boost::iterator_facade
	<

		CoreRowsPointer<T, TRANSPOSED>      ,
		CoreRow<T, TRANSPOSED>              ,
		boost::random_access_traversal_tag  ,
		CoreRow<T, TRANSPOSED>

	> iterator_facade_type;

	MYSTD_ITERATOR_FACADE_INTRODUCE(iterator_facade_type)

	// MYSTD_ITERATOR_FACADE_INTERFACE                                      // $2

  void            increment   ()                          throw();
	void            decrement   ()                          throw();
	void            advance     (difference_type)           throw();

	template<typename TT>
	difference_type distance_to (CoreRowsPointer<TT, TRANSPOSED> const &)
	const throw();

	template<typename TT>
	bool            equal       (CoreRowsPointer<TT, TRANSPOSED> const &)
	const throw();

	reference       dereference () const throw();

	// <

	ECHMET_MATRIX_COREROWS_POINTER_FRIENDS

	RowsPointer<T, TRANSPOSED> ptr;

public:

	// Constructors

					 CoreRowsPointer()                                      throw();
	explicit CoreRowsPointer(RowsPointer<T, TRANSPOSED>  const &)   throw();

	// $2
	CoreRowsPointer(CoreRowsPointer const &) throw();

	// $2
	template <typename TT>
	CoreRowsPointer (

    CoreRowsPointer <TT, TRANSPOSED> const & ,
    typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

  ) throw();

  // Assignment
	// $2

  CoreRowsPointer & operator=(CoreRowsPointer const &) throw();

  template <typename TT>
  typename boost::enable_if
	< boost::is_convertible<TT *, T *>, CoreRowsPointer & >
  ::type
  operator=( CoreRowsPointer <TT, TRANSPOSED> const & ) throw();

  // STD

  void swap(CoreRowsPointer &) throw();

};

//-----------------------------------------------------------------------------
// STD

template<typename T, transtags_ DT>
void swap( CoreRowsPointer<T, DT> &, CoreRowsPointer<T, DT> & ) throw();

//=============================================================================
}}} // namespace echmet :: matrix :: impl

#endif // Header
