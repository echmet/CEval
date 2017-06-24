#ifndef ECHMET_MATRIX_CELLS_POINTERH
#define ECHMET_MATRIX_CELLS_POINTERH
//=============================================================================
// COMMENTS
//

// $ 1) -> $ C1) in fwdcl.h

// $ 2) -> $ C2) in fwdcl.h

// $ 3) MYSTD_ITERATOR_FACADE_INTRODUCE macro does not work in RAD XE3 here

// $ 4) -> $ C4) in fwdcl.h

//=============================================================================
// INCLUDES

#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>

#include "../../mystd/iterator_facade_ext.h"

#include "../fwdcl.h"

//=============================================================================
// MACROS

// $ 1
#define ECHMET_MATRIX_CELLS_POINTER_FRIENDS                                    \
  template<typename TT, transtags_ DTT>    friend class CellsPointer;           \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// trNormal

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
class CellsPointer<T, trNormal> : public boost::iterator_facade
<
  CellsPointer<T, trNormal>           ,
	T                                   ,
	boost::random_access_traversal_tag  ,
	T &
>
{
private:

	// Defs

	typedef boost::iterator_facade <
		CellsPointer<T, trNormal>          ,
		T                                  ,
		boost::random_access_traversal_tag ,
		T &
	> iterator_facade_type;

	// MYSTD_ITERATOR_FACADE_INTRODUCE(iterator_facade_type)               // $3

	friend class boost::iterator_core_access;

	public:

	typedef T                value_type;
	typedef T &              reference;
	typedef T *              pointer;
	typedef ::std::ptrdiff_t difference_type;
	typedef typename iterator_facade_type::iterator_category iterator_category;

  private:

  // <

  // MYSTD_ITERATOR_FACADE_INTERFACE                                     // $4

  void            increment   ()                          throw();
  void            decrement   ()                          throw();
  void            advance     (difference_type)           throw();

  template<typename TT>
  difference_type distance_to (CellsPointer<TT, trNormal> const &)
  const throw();

  template<typename TT>
  bool            equal       (CellsPointer<TT, trNormal> const &)
  const throw();

  reference       dereference () const throw();

  // <

  ECHMET_MATRIX_CELLS_POINTER_FRIENDS

  // Data

  T * ptr;

public:

  // Constructors & Assignment

  CellsPointer()                throw();
  CellsPointer(T *, mptrdiff_t) throw();

  // $2
  CellsPointer(CellsPointer const & ) throw();

  // $2
  template <typename TT>
  CellsPointer (

	CellsPointer <TT, trNormal> const & ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

  ) throw();

	// Assignment
	// $2

  CellsPointer & operator=(CellsPointer const &) throw();

  template <typename TT>
  typename boost::enable_if< boost::is_convertible<TT *, T *>, CellsPointer & >
  ::type
  operator=( CellsPointer<TT, trNormal> const & ) throw();

  // STD

  void swap(CellsPointer &) throw();

};

//-----------------------------------------------------------------------------
// trTransposed

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
class CellsPointer<T, trTransposed> : public boost::iterator_facade
<
	CellsPointer<T, trTransposed>      ,
	T                                  ,
	boost::random_access_traversal_tag ,
	T &
>
{
private:

  // Defs

  typedef boost::iterator_facade <
		CellsPointer<T, trTransposed>          ,
		T                                      ,
		boost::random_access_traversal_tag     ,
		T &
	> iterator_facade_type;

  // MYSTD_ITERATOR_FACADE_INTERFACE                                     // $4

  friend class boost::iterator_core_access;

  public:

  typedef T                value_type;
	typedef T &              reference;
  typedef T *              pointer;
  typedef ::std::ptrdiff_t difference_type;
  typedef typename iterator_facade_type::iterator_category iterator_category;

  private:

  // <

	// MYSTD_ITERATOR_FACADE_INTERFACE                                     // $3

  void            increment   ()                          throw();
  void            decrement   ()                          throw();
  void            advance     (difference_type)           throw();

  template<typename TT>
  difference_type distance_to (CellsPointer<TT, trTransposed> const &)
  const throw();

  template<typename TT>
  bool            equal       (CellsPointer<TT, trTransposed> const &)
  const throw();

  reference       dereference () const throw();

  // <

  ECHMET_MATRIX_CELLS_POINTER_FRIENDS

  // Data

  T **       rows;
  mptrdiff_t i;

public:

  // Constructors & Assignment

  CellsPointer()                                   throw();
  CellsPointer(T ** arows, mptrdiff_t, mptrdiff_t) throw();

  CellsPointer(CellsPointer const &) throw();                             // $2

  template <typename TT>                                                  // $2
  CellsPointer (

	CellsPointer <TT, trTransposed> const & ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

  ) throw();

  CellsPointer & operator=(CellsPointer const &) throw();                 // $2

  template <typename TT>                                                  // $2
  typename boost::enable_if< boost::is_convertible<TT *, T *>, CellsPointer & >
  ::type
  operator=( CellsPointer<TT, trTransposed> const & other ) throw();

  // STD

  void swap(CellsPointer &) throw();

};

//-----------------------------------------------------------------------------
// trDiagonal

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
class CellsPointer<T, trDiagonal> : public boost::iterator_facade
<
	CellsPointer<T, trDiagonal>        ,
	T                                  ,
	boost::random_access_traversal_tag ,
	T &
>
{
private:

	// Defs

	typedef boost::iterator_facade <
		CellsPointer<T, trDiagonal>        ,
		T                                  ,
		boost::random_access_traversal_tag ,
		T &
	> iterator_facade_type;

	// MYSTD_ITERATOR_FACADE_INTRODUCE(iterator_facade_type)               // $3

	friend class boost::iterator_core_access;

	public:

	typedef T                value_type;
	typedef T &              reference;
	typedef T *              pointer;
	typedef ::std::ptrdiff_t difference_type;
	typedef typename iterator_facade_type::iterator_category iterator_category;

	private:

  // <

  // MYSTD_ITERATOR_FACADE_INTERFACE                                     // $4

  void            increment   ()                          throw();
  void            decrement   ()                          throw();
  void            advance     (difference_type)           throw();

  template<typename TT>
  difference_type distance_to (CellsPointer<TT, trDiagonal> const &)
  const throw();

  template<typename TT>
  bool            equal       (CellsPointer<TT, trDiagonal> const &)
  const throw();

  reference       dereference () const throw();

  // <

  ECHMET_MATRIX_CELLS_POINTER_FRIENDS

  // Data

  T **       rows;
  mptrdiff_t i;

public:

  // Constructors & Assignment

  CellsPointer()                 throw();
  CellsPointer(T **, mptrdiff_t) throw();

  CellsPointer(CellsPointer const & ) throw();                            // $2

  template <typename TT>                                                  // $2
  CellsPointer (

	CellsPointer <TT, trDiagonal> const & ,
  typename boost::enable_if< boost::is_convertible<TT *, T *> >::type * = NULL

  ) throw();

  CellsPointer & operator=(CellsPointer const &) throw();                 // $2

  template <typename TT>                                                  // $2
  typename boost::enable_if
  < boost::is_convertible<TT *, T *>, CellsPointer & >
  ::type
  operator=( CellsPointer<TT, trDiagonal> const & ) throw();

  // STD

  void swap(CellsPointer &) throw();

};

//-----------------------------------------------------------------------------
// > STD >
//-----------------------------------------------------------------------------

template<typename T, transtags DT>
void swap( CellsPointer<T, DT> &, CellsPointer<T, DT> & ) throw();

//==============================================================================

}}} // namespace echmet :: matrix :: impl

#endif // Header



