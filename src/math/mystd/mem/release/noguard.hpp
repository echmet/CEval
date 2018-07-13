#ifndef MYSTD_MEM_RELEASE_NOGUARD_HPP
#define MYSTD_MEM_RELEASE_NOGUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------

// Includes:

#include <boost\type_traits\has_trivial_destructor.hpp>

#ifndef MYSTD_MEM_BASE_HPP
#  include "_SHARED\MyStd\mem\base.hpp"
#endif

#ifndef MYSTD_MEM_RELEASE_ADVANCE_HPP
#  include "_SHARED\MyStd\mem\release\advance.hpp"
#endif

#ifndef MYSTD_MEM_RELEASE_NOGUARD_H
#  include "_SHARED\MyStd\mem\release\noguard.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------

// Definitions:

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release(
  typename Allocator::pointer   p,
  typename Allocator::size_type sz,
  Allocator                     a
)
{
  return Release<T, Allocator>(p, sz, sz, a);
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release(
  typename Allocator::pointer   p,
  typename Allocator::size_type sz,
  typename Allocator::size_type loff,
  Allocator                     a
)
{
	return Release<T, Allocator>(p, sz, AdvanceForRelease<T, Allocator>(p, loff), loff, a);
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release(
	typename Allocator::pointer   p,
	typename Allocator::size_type sz,
	typename Allocator::pointer   lp,
	Allocator                     a
)
{
	return Release<T, Allocator>(p, sz, lp, lp - p, a);
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release(
  typename Allocator::pointer   p,
  typename Allocator::size_type sz,
  typename Allocator::pointer   lp,
  typename Allocator::size_type loff,
  Allocator                     a
)
{
  
  inside::release_noguard_helper<
    T, Allocator,
    boost::has_trivial_destructor<typename Allocator::value_type>::value
  >::destroy(lp, loff, a);
  
  return Dealloc<T, Allocator>(p, sz, a);  

}

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_destructables
template<typename T, typename Allocator>
struct release_noguard_helper<T, Allocator, true>
{

	// throw
	static inline void
	destroy(
		typename Allocator::pointer   myStd_dummy,
		typename Allocator::size_type myStd_dummy,
		Allocator                     myStd_dummy
	 )
	{  }

};

//-----------------------------------------------------------------------------
// non-default_destructables
template<typename T, typename Allocator>
struct release_noguard_helper<T, Allocator, false>
{

  static inline void 
  destroy( 
    typename Allocator::pointer     lp,
    typename Allocator::size_type   loff,
    Allocator                       a 
  )
  {
    for (; loff != 0; --loff ) a.destroy(--lp);
  }

};
//-----------------------------------------------------------------------------
  
} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

