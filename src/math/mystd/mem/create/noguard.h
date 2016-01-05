#ifndef MYSTD_MEM_CREATE_NOGUARD_H
#define MYSTD_MEM_CREATE_NOGUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Includes:

#include "_SHARED\MyStd\_MyStdUnit.set"
#include "_SHARED\MyStd\stddefs.h"

#include "_SHARED\MyStd\mem\base.h"

//=============================================================================
//-----------------------------------------------------------------------------

namespace myStd {

namespace mem {

//=============================================================================
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Does not invoke constructor for trivial_constructables
// uses default ctor otherwise
template<typename T, typename Allocator>
inline typename Allocator::pointer Create(
  typename Allocator::size_type = 1,
  Allocator                     = Allocator(),
  typename Allocator::pointer   = allocator_extensions<Allocator>::null_type() 
);

//-----------------------------------------------------------------------------
// Initializes each array element with const_reference
template<typename T, typename Allocator>
inline typename Allocator::pointer Create(
  typename Allocator::const_reference,                            
  typename Allocator::size_type       = 1,
  Allocator                           = Allocator(),
  typename Allocator::pointer         = allocator_extensions<Allocator>::null_type() 
);

//-----------------------------------------------------------------------------
// Copies an array from const_pointer
// The array must be of sufficient size
// Makes a copy (neither move nor swap)
// T must be primitive_constructible
// or Allocator::const_pointer must additionally satisfy ForwardIterator concept
template<typename T, typename Allocator>
inline typename Allocator::pointer Create(
  typename Allocator::const_pointer,                            
  typename Allocator::size_type    ,
  Allocator                        = Allocator(),
  typename Allocator::pointer      = allocator_extensions<Allocator>::null_type() 
);
//-----------------------------------------------------------------------------

// ---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Create(
  std::nothrow_t,
  typename Allocator::size_type = 1,
  Allocator                     = Allocator(),
  typename Allocator::pointer   = allocator_extensions<Allocator>::null_type() 
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Create(
	std::nothrow_t,
	typename Allocator::const_reference,
	typename Allocator::size_type       = 1,
	Allocator                           = Allocator(),
	typename Allocator::pointer         = allocator_extensions<Allocator>::null_type()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Create(
  std::nothrow_t,
  typename Allocator::const_pointer,                            
  typename Allocator::size_type    ,
  Allocator                        = Allocator(),
  typename Allocator::pointer      = allocator_extensions<Allocator>::null_type() 
);
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > mpl helpers  >

//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct create_noguard_helper;

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct create_noguard_throw_helper;

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct create_noguard_nothrow_helper;

//-----------------------------------------------------------------------------

// < mpl helpers <

//-----------------------------------------------------------------------------

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif //header
