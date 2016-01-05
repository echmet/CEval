#ifndef MYSTD_ITERATOR_FACADE_EXT_H
#define MYSTD_ITERATOR_FACADE_EXT_H
//==============================================================================
// COMMENTS

//==============================================================================
// INCLUDES

#include <boost/iterator/iterator_facade.hpp>

//=============================================================================
// MACROS

#define MYSTD_ITERATOR_FACADE_INTRODUCE(FACADE)                                \
																			                                         \
  friend class boost::iterator_core_access;                                    \
																			                                         \
  public:                                                                      \
																			                                         \
  typedef typename FACADE::value_type        value_type;                       \
  typedef typename FACADE::reference         reference;                        \
  typedef typename FACADE::pointer           pointer;                          \
  typedef typename FACADE::difference_type   difference_type;                  \
  typedef typename FACADE::iterator_category iterator_category;                \
																			                                         \
  private:                                                                     \

// This macro is not applicable when [const]-iterator in not
// template based (i.e. iterator == THIS<T> -> const_iterator == THIS<const T>)
// or THIS takes more than one template parameter
// The code below can still be then used the iterator's interface pattern
#define MYSTD_ITERATOR_FACADE_INTERFACE(THIS)                                  \
                                                                               \
  void            increment   ()                          throw();             \
  void            decrement   ()                          throw();             \
  void            advance     (difference_type)           throw();             \
                                                                               \
  template<typename MYSTD_ITERATOR_FACADE_INTERFACE_T>                         \
  difference_type distance_to (THIS<MYSTD_ITERATOR_FACADE_INTERFACE_T> const &)\
  const throw();                                                               \
                                                                               \
  template<typename MYSTD_ITERATOR_FACADE_INTERFACE_T>                         \
  bool            equal       (THIS<MYSTD_ITERATOR_FACADE_INTERFACE_T> const &)\
  const throw();                                                               \
                                                                               \
  reference       dereference () const throw();                                \





//=============================================================================
// NAMESPACE

namespace myStd {

//=============================================================================
// DCL

//=============================================================================
// DEF

//==============================================================================
} // namespace myStd

#endif // Header

