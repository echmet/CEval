#ifndef MYSTD_MEM_CREATE_GUARD_H
#define MYSTD_MEM_CREATE_GUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:

/* 
 * Read Help.txt 
 */

//-----------------------------------------------------------------------------

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
inline Guard<T, Allocator> &
Create(
  Guard<T, Allocator> &, 
  Allocator            = Allocator()
);

//-----------------------------------------------------------------------------
// Initializes each array element with const_reference
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  typename Allocator::const_reference,
  Guard<T, Allocator> &              ,
  Allocator            = Allocator()
);

//-----------------------------------------------------------------------------
// Copies an array from const_pointer
// The array must be of sufficient size
// Makes a copy (neither move nor swap)
// T must be primitive_constructible
// or Allocator::const_pointer must additionally satisfy ForwardIterator concept
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  typename Allocator::const_pointer,
  Guard<T, Allocator> &            ,
  Allocator                        = Allocator()
);
//-----------------------------------------------------------------------------

//---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  std::nothrow_t        ,
  Guard<T, Allocator> & ,
  Allocator             = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  std::nothrow_t                     ,
  typename Allocator::const_reference,  
  Guard<T, Allocator> &              ,
  Allocator                          = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  std::nothrow_t                   ,
  typename Allocator::const_pointer, 
  Guard<T, Allocator> &            ,
  Allocator                        = Allocator()
);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

// > mpl helpers  >

//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct create_guard_helper;

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct create_guard_nothrow_helper;

//-----------------------------------------------------------------------------

// < mpl helpers <

//-----------------------------------------------------------------------------

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif //header
