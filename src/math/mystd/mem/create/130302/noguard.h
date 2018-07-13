#ifndef MYSTD_MEM_CREATE_NOGUARD_H
#define MYSTD_MEM_CREATE_NOGUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:

/* All non-guarded Create functions are automatically double-nothrow versions
 * as otherwise there would be no way to safely destroy all
 * possible objects that have been created in a field 
 * prior the object causing an exception.
 *
 * In case of unsuccessful mem initialization a rollback action takes place
 * LIFO-destroying all the so far successfully constructed objects
 * in the field, and freeing the allocated memory.
 * An exception may still be thrown out by destructor or deallocator in this case.
 * => this basically indicates a critical program error condition as an undefined
 *    number of objects may be left undestroyed and the memory is unfreed.
 * 
 * Requirements :
 * 
 * The both guard and non-guard implementations require that Allocator::pointer 
 * be a ForwardIterator
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
inline typename Allocator::pointer Create(
  typename Allocator::size_type = 1,
  typename Allocator::pointer   = allocator_extensions<Allocator>::null_type() 
);

//-----------------------------------------------------------------------------
// Initializes each array element with const_reference
template<typename T, typename Allocator = allocator<T> >
inline typename Allocator::pointer Create(
  typename Allocator::const_reference,                            
  typename Allocator::size_type       = 1,
  typename Allocator::pointer         = allocator_extensions<Allocator>::null_type() 
);

//-----------------------------------------------------------------------------
// Copies an array from const_pointer
// The array must be of sufficient size
// Makes a copy (neither move nor swap)
// T must be primitive_constructible
// or Allocator::const_pointer must additionally satisfy ForwardIterator concept
template<typename T, typename Allocator = allocator<T> >
inline typename Allocator::pointer Create(
  typename Allocator::const_pointer,                            
  typename Allocator::size_type       = 1,
  typename Allocator::pointer         = allocator_extensions<Allocator>::null_type() 
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
