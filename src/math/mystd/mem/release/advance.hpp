#ifndef MYSTD_MEM_RELEASE_ADVANCE_HPP
#define MYSTD_MEM_RELEASE_ADVANCE_HPP

//=============================================================================
//-----------------------------------------------------------------------------

// Includes:

#include <boost\type_traits\has_trivial_destructor.hpp>

//=============================================================================
//-----------------------------------------------------------------------------

// Definitions:

namespace myStd {

namespace mem {

template<typename T, typename Allocator>
typename Allocator::pointer
AdvanceForRelease(
	typename Allocator::pointer   lp,
	typename Allocator::size_type loff
)
{
	return inside::advance_helper<
		T, Allocator,
		boost::has_trivial_destructor<typename Allocator::value_type>::value
	>::advance(lp, loff);
}

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_destructibles
template<typename T, typename Allocator>
struct advance_helper<T, Allocator, true> {

  static inline typename Allocator::pointer 
  advance(typename Allocator::pointer lp, typename Allocator::size_type loff)
  { return lp; }

};

//-----------------------------------------------------------------------------
// non-default_destructibles
template<typename T, typename Allocator>
struct advance_helper<T, Allocator, false> {

  static inline typename Allocator::pointer 
  advance(typename Allocator::pointer lp, typename Allocator::size_type loff)
  {  return lp + loff; }

};

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

//-----------------------------------------------------------------------------
#endif // Header