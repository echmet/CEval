#ifndef MYSTD_MEM_BASE_HPP
#define MYSTD_MEM_BASE_HPP

#include <cstring> // memcpy

#include <boost\type_traits\has_nothrow_copy.hpp>

#ifndef MYSTD_MEM_BASE_H
#  include "_SHARED\MyStd\mem\base.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------

// Definitions:

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Alloc(
  typename Allocator::size_type     sz,
  Allocator                         a,
  typename Allocator::const_pointer hint
)
{
  return sz > 0 ? a.allocate(sz, hint) : Allocator::pointer(allocator_extensions<Allocator>::null_type());
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Alloc(
  std::nothrow_t                    myStd_dummy,
  typename Allocator::size_type     sz,
  Allocator                         a,
  typename Allocator::const_pointer hint
)
{
  return 
    sz > 0                                                               ? 
    allocator_extensions<Allocator>::allocate(std::nothrow, sz, hint, a) : 
    allocator_extensions<Allocator>::null_type()
  ;
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Dealloc(
  typename Allocator::pointer   p,
  typename Allocator::size_type sz,
  Allocator                     a
)
{
	return inside::dealloc_helper<
		T, Allocator,
		allocator_extensions<Allocator>::has_nothrow_deallocator::value
	>::deallocate(p, sz, a);
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Dealloc(
	std::nothrow_t                myStd_dummy,
	typename Allocator::pointer   p,
	typename Allocator::size_type sz,
	Allocator                     a
)
{
  return inside::dealloc_helper<
    T, Allocator,
    allocator_extensions<Allocator>::has_nothrow_deallocator::value
  >::deallocate(std::nothrow, p, sz, a);
}

//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > allocator_extensions<allocator> >

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<typename T> 
inline typename allocator_extensions<std::allocator<T> >::pointer
allocator_extensions<std::allocator<T> >::allocate(
	std::nothrow_t    myStd_dummy,
	size_type         sz,
	const_pointer     hint,
	std::allocator<T> a
)
{
	pointer p = allocator_extensions<std::allocator<T> >::null_type();

	try        { if (sz != 0) a.allocate(sz, hint); }
	catch(...) {  }

	return p;
}

//-----------------------------------------------------------------------------
template<typename T>
inline typename allocator_extensions<std::allocator<T> >::size_type
allocator_extensions<std::allocator<T> >::rawcopy(
  typename std::allocator<T>::const_reference t, 
  typename std::allocator<T>::pointer         p, 
  typename std::allocator<T>::size_type       sz,
  std::allocator<T>                           a
)
{
  return allocator_rawcopy_helper<
    boost::has_nothrow_copy_constructor<typename std::allocator<T>::value_type>::value    
  >::rawcopy(t, p, sz, a);
}

//-----------------------------------------------------------------------------
template<typename T> 
inline typename allocator_extensions<std::allocator<T> >::size_type
allocator_extensions<std::allocator<T> >::rawcopy(
  typename std::allocator<T>::const_pointer   src, 
  typename std::allocator<T>::pointer         p, 
  typename std::allocator<T>::size_type       sz,
  std::allocator<T>                           myStd_dummy
)
{
  std::memcpy((void *)(p), (const void *)(src), sz * sizeof(T));
  return sz;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < allocator_extensions<allocator> <

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {


//-----------------------------------------------------------------------------
// dealloc, has nothrow deallocator
template<typename T, typename Allocator>
struct dealloc_helper<T, Allocator, true> {

  // throw
  static inline typename Allocator::pointer deallocate(
    typename Allocator::pointer   p,
    typename Allocator::size_type sz,
    Allocator                     a
  )
  {
    a.deallocate(p, sz);
    return allocator_extensions<Allocator>::null_type();
  }

  // nothrow
  static inline typename Allocator::pointer deallocate(
    std::nothrow_t                myStd_dummy,
    typename Allocator::pointer   p,
    typename Allocator::size_type sz,
    Allocator                     a
  )
  {
    return deallocate(p, sz, a);
  }

};

//-----------------------------------------------------------------------------
// dealloc, has throw deallocator
template<typename T, typename Allocator>
struct dealloc_helper<T, Allocator, false> {

  // throw
  static inline typename Allocator::pointer deallocate(
    typename Allocator::pointer   p,
    typename Allocator::size_type sz,
    Allocator                     a
  )
  {
    return dealloc_helper<T, Allocator, true>::deallocate(p, sz, a);
  }

  // nothrow
  static inline typename Allocator::pointer deallocate(
    std::nothrow_t                myStd_dummy,
    typename Allocator::pointer   p,
    typename Allocator::size_type sz,
    Allocator                     a
  )
  {
    try        { return deallocate(p, sz, a); }
    catch(...) { return p; }
  }

};

// ---

//-----------------------------------------------------------------------------
// allocator_extensions<std::allocator>, rawcopy, nothrow_copy
template<typename T>
struct allocator_rawcopy_helper<T, true> {

  static inline typename std::allocator<T>::pointer rawcopy(
    typename std::allocator<T>::const_reference t,
    typename std::allocator<T>::pointer         p,
    typename std::allocator<T>::size_type       sz,
    std::allocator<T>                           a
  )
  {
    for (typename std::allocator<T>::size_type loff = 0; loff != sz; ++loff, ++p) a.construct(p, t);
  }

};

//-----------------------------------------------------------------------------
// allocator_extensions<std::allocator>, rawcopy, throw_copy
template<typename T>
struct allocator_rawcopy_helper<T, false> {

  static inline typename std::allocator<T>::pointer rawcopy(
    typename std::allocator<T>::const_reference t,
    typename std::allocator<T>::pointer         p,
    typename std::allocator<T>::size_type       sz,
    std::allocator<T>                           a
  )
  {

    typename std::allocator<T>::size_type loff;

    try        { for (loff = 0; loff != sz; ++loff, ++p) a.construct(p, t); }
    catch(...) {  }

    return loff;

  }

};

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

//-----------------------------------------------------------------------------
#endif // header

