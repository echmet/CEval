#ifndef MYSTD_MEM_CREATE_GUARD_H
#define MYSTD_MEM_CREATE_GUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:

/* Single-nothrow overloads
 * 
 * Use this overloaded version of Create function if you don't want to catch 
 * any allocation related exception but you are still interested in possible 
 * object constructor's exceptions. 
 * - Utilise Guard::operator bool() to learn whether allocation was successful
 *   instead of catching allocation related exceptions. 
 * - note: an exception may still be thrown from default constructor if 
 *   default parameter T() is used
 *     
 * Double-nothrow overloads
 *
 * Use this overloaded version of Create function if you don't want to catch 
 * any allocation related exception and neither you are interested in possible 
 * object constructor's exceptions. 
 * - Utilise Guard::operator bool() to learn whether allocation and construction
 *   of all objects were successful instead of catching related exceptions.
 * - note: an exception may still be thrown from default constructor if 
 *   default parameter T() is used
 *              
 * Result:
 *  
 *  if !Guard :
 *    - Guard.lp == NULL : 
 *      Either memory could not be allocated or exception while memory filling
 *      => No object is left constructed undestroyed
 *         No memory is allocated   
 *    - Guard.lp != NULL 
 *      - Guard.loff != 0
 *        - not-nothrow and single-nothrow versions:  
 *          Exception when constructing an object at offset p.loff, address p.lp
 *        - double-nothrow version:
 *          Exception when destructing an object at offset p.loff, address p.lp              
 *        => Objects up to p.lp are existing undestroyed
 *           Memory is allocated    
 *      - Guard.loff == 0
 *        Deallocation failed
 *        => No object is left constructed undestroyed
 *           Memory is allocated 
 * 
 *  Preconditions:
 *  
 *  The Create function does not clear the Guard    
 *  The correct result is only guaranteed if a newly constructed Guard is provided
 *  as the function parameter.
 *  .p can be compared against NULL instead of .lp only in case of 
 *  the Guard has not been constructed with the hint parameter.        
 *
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
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create( Guard<T, Allocator> & )

//-----------------------------------------------------------------------------
// Initializes each array element with const_reference
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create( typename Allocator::const_reference, Guard<T, Allocator> &);

//-----------------------------------------------------------------------------
// Copies an array from const_pointer
// The array must be of sufficient size
// Makes a copy (neither move nor swap)
// T must be primitive_constructible
// or Allocator::const_pointer must additionally satisfy ForwardIterator concept
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create( typename Allocator::const_pointer, Guard<T, Allocator> &);
//-----------------------------------------------------------------------------

//---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create(
  std::nothrow_t,
  Guard<T, Allocator> &
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create(
  std::nothrow_t,
  typename Allocator::const_reference, 
  Guard<T, Allocator> &
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create(
  std::nothrow_t,
  typename Allocator::const_pointer, 
  Guard<T, Allocator> &
);
//-----------------------------------------------------------------------------

//---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create(
  std::nothrow_t, std::nothrow_t,
  Guard<T, Allocator> &
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create(
  std::nothrow_t, std::nothrow_t,
  typename Allocator::const_reference,  
  Guard<T, Allocator> &
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = allocator<T> >
inline Guard<T, Allocator> & Create(
  std::nothrow_t, std::nothrow_t,
  typename Allocator::const_pointer, 
  Guard<T, Allocator> &
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
