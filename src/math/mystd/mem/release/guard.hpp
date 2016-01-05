#ifndef MYSTD_MEM_RELEASE_GUARD_HPP
#define MYSTD_MEM_RELEASE_GUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------

#include <boost\type_traits\has_trivial_destructor.hpp>

#ifndef MYSTD_MEM_BASE_HPP
#  include "_SHARED\MyStd\mem\base.hpp"
#endif

#ifndef MYSTD_MEM_RELEASE_ADVANCE_HPP
#  include "_SHARED\MyStd\mem\release\advance.h"
#endif

#ifndef MYSTD_MEM_RELEASE_GUARD_H
#  include "_SHARED\MyStd\mem\release\guard.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Release( 
  Guard<T, Allocator> & g,
  Allocator             a 
)
{
  
  inside::release_guard_helper<
    T, Allocator,
    boost::has_trivial_destructor<typename Allocator::value_type>::value
  >::destroy(g, a);
  
  g.p  = g.lp = Dealloc<T, Allocator>(g.p, g.sz, a);
  g.sz = 0;
  
  return g;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Release( 
  std::nothrow_t        myStd_dummy,
  Guard<T, Allocator> & g, 
  Allocator             a 
)
{
  
  inside::release_guard_helper<
    T, Allocator,
    boost::has_trivial_destructor<typename Allocator::value_type>::value
  >::destroy(std::nothrow, g, a);
  
  if (g.loff == 0) g.p  = g.lp = Dealloc<T, Allocator>(std::nothrow, g.p, g.sz, a); 
  if (!g.p)        g.sz = 0;
  
  return g;

}


//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_destructables
template<typename T, typename Allocator>
struct release_guard_helper<T, Allocator, true>
{

	// throw
	static inline void
	destroy( Guard<T, Allocator> & myStd_dummy, Allocator myStd_dummy )
	{  }

	// nothrow
	static inline void
	destroy(
		std::nothrow_t        myStd_dummy,
		Guard<T, Allocator> & myStd_dummy,
		Allocator             myStd_dummy
	)
	{  }

};

//-----------------------------------------------------------------------------
// non-default_destructables
template<typename T, typename Allocator>
struct release_guard_helper<T, Allocator, false>
{

	// throw
	static inline void
	destroy( Guard<T, Allocator> & g, Allocator a )
	{

		for (; g.loff != 0; --(g.loff) ) a.destroy(--(g.lp));

		return g;

	}

  // nothrow
  static inline void 
  destroy(
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g,
    Allocator             a
  )
  {
    try        { destroy(g, a); }
    catch(...) { }
  }

};
//-----------------------------------------------------------------------------
  
} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

