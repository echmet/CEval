#ifndef MYSTD_MEM_RELEASE_NOGUARD_H
#define MYSTD_MEM_RELEASE_NOGUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:
/*
 * nothrow overload does not exists
 * - there is no simple return value to represent whether all desrtructions 
 *   and the final deallocation was successful
 * - use guarded version instead, if needed
 */        


//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

// Includes:

#include "_SHARED\MyStd\_MyStdUnit.set"
#include "_SHARED\MyStd\stddefs.h"

#include "_SHARED\MyStd\mem\base.h"

#include "_SHARED\MyStd\mem\release\advance.h"

//=============================================================================
//-----------------------------------------------------------------------------

// Declarations:

namespace myStd {

namespace mem {

//=============================================================================
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release (
	typename Allocator::pointer  ,
	typename Allocator::size_type,
	Allocator                    = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release (
	typename Allocator::pointer  ,
	typename Allocator::size_type,
	typename Allocator::size_type,
	Allocator                    = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release (
	typename Allocator::pointer  ,
	typename Allocator::size_type,
	typename Allocator::pointer  ,
	Allocator                    = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer
Release (
	typename Allocator::pointer  ,
  typename Allocator::size_type, 
  typename Allocator::pointer  ,
  typename Allocator::size_type, 
  Allocator                    = Allocator() 
);

//-----------------------------------------------------------------------------

// > mpl helpers  >

//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct release_noguard_helper;

} // namespace

//-----------------------------------------------------------------------------

// < mpl helpers <

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif //header
