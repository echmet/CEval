#ifndef MYSTD_MEM_COPY_NOGUARD_HPP
#define MYSTD_MEM_COPY_NOGUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------

#include <boost\type_traits\has_trivial_assign.hpp>
#include <boost\type_traits\has_nothrow_assign.hpp>
#include <boost\type_traits\detail\ice_and.hpp>

#ifndef MYSTD_MEM_BASE_HPP
#  include "_SHARED\MyStd\mem\base.hpp"
#endif

#ifndef MYSTD_MEM_COPY_NOGUARD_H
#  include "_SHARED\MyStd\mem\copy\noguard.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type 
Copy( 
  typename Allocator::const_reference t, 
  typename Allocator::pointer         p, 
  typename Allocator::size_type       sz,
  Allocator                           a
)
{
  return 
    inside::copy_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(t, p, sz, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type 
Copy( 
  typename Allocator::const_pointer  src, 
  typename Allocator::pointer        p, 
  typename Allocator::size_type      sz,
	Allocator                          a
)
{
  return 
    inside::copy_noguard_helper<
      T, Allocator,
      boost::type_traits::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(src, p, sz, a)
  ;

}
//-----------------------------------------------------------------------------

// ---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type 
Copy( 
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_reference t, 
  typename Allocator::pointer         p, 
  typename Allocator::size_type       sz,
  Allocator                           a
)
{
  return 
    inside::copy_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(std::nothrow, t, p, sz, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type 
Copy( 
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_pointer   src, 
  typename Allocator::pointer         p, 
  typename Allocator::size_type       sz,
  Allocator                           a
)
{
  return 
    inside::copy_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(std::nothrow, src, p, sz, a)
  ;

}

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_assignables
template<typename T, typename Allocator>
struct copy_noguard_helper<T, Allocator, true>
{

  // reference, throw (does not throw, anyway) 
  static inline typename Allocator::size_type 
  copy( 
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {
    
    typename Allocator::size_type loff = copy(std::nothrow, t, p, sz, a);
    if (loff != sz) throw MYSTD_TMPERR;
    
    return sz;

  }
  
  // field, throw (does not throw anyway)
  static inline typename Allocator::size_type 
  copy( 
    typename Allocator::const_pointer   src, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {

    typename Allocator::size_type loff = copy(std::nothrow, src, p, sz, a);
    if (loff != sz) throw MYSTD_TMPERR;
    
    return sz;

  }
  
  
  // reference, nothrow
  static inline typename Allocator::size_type 
  copy(
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz, 
    Allocator                           a
  )
  {
    return allocator_extensions<Allocator>::rawcopy(t, p, sz, a);  
  }
  
  // field, nothrow
  static inline typename Allocator::size_type 
  copy(
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_pointer   src, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {
    return allocator_extensions<Allocator>::rawcopy(src, p, sz, a);  
  }
  
};

//-----------------------------------------------------------------------------
// non-default_assignables
template<typename T, typename Allocator>
struct copy_noguard_helper<T, Allocator, false>
{

  // reference, throw
  static inline typename Allocator::size_type 
  copy(
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           myStd_dummy
  )
  {
    for (Allocator::size_type loff = 0; loff != sz; ++loff, ++p ) *p = t; 
    return sz;                                                            // 1  
  }
  // 1 : either copy all or throw
  
  // field, throw
  static inline typename Allocator::size_type 
  copy(
    typename Allocator::const_pointer   src, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           myStd_dummy
  )
  {
    for (Allocator::size_type loff = 0; loff != sz; ++loff, ++p, ++src ) *p = *src; 
    return sz;                                                            // 1  
  }
  // 1 : either copy all or throw
  
  // reference, nothrow
  static inline typename Allocator::size_type 
  copy(
    std::nothrow_t                      myStd_dummy,
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {
    return copy_noguard_nothrow_helper<
      boost::has_nothrow_assign<typename Allocator::value_type>::value
    >::copy(t, p, sz, a);  
  }
  
  // field, nothrow
  static inline typename Allocator::size_type 
  copy(
    std::nothrow_t                      myStd_dummy,
    typename Allocator::const_pointer   src, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {
    return copy_noguard_nothrow_helper<
      boost::has_nothrow_assign<typename Allocator::value_type>::value
    >::copy(src, p, sz, a);  
  }
  
};

//-----------------------------------------------------------------------------
// non-default_assignables, nothrow_helper, nothrow_assignables
template<typename T, typename Allocator>
struct copy_noguard_nothrow_helper<T, Allocator, true>
{

  // reference
  static inline typename Allocator::size_type 
  copy(
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {
    return copy_guard_helper<T, Allocator, false>::copy(t, p, sz, a);
  }
  
  // field
  static inline typename Allocator::size_type 
  copy(
    typename Allocator::const_pointer   src, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           a
  )
  {
    return copy_guard_helper<T, Allocator, false>::copy(src, p, sz, a);
  }
  
};

//-----------------------------------------------------------------------------
// non-default_assignables, nothrow_helper, throw_asignables
template<typename T, typename Allocator>
struct copy_noguard_nothrow_helper<T, Allocator, false>
{

  // reference
  static inline typename Allocator::size_type 
  copy(
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           myStd_dummy
  )
  {
        
    Allocator::size_type loff;
    try        { for (loff = 0; loff != sz; ++loff, ++p ) *p = t; }
    catch(...) {} 
    
    return loff;  
  
  }
  
  // field
  static inline typename Allocator::size_type 
  copy(
    typename Allocator::const_pointer   src, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz,
    Allocator                           myStd_dummy
  )
  {
        
    Allocator::size_type loff;
    try        { for (loff = 0; loff != sz; ++loff, ++p, ++src ) *p = *src; }
    catch(...) {} 
    
    return loff;  
  
  }
  
};

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

