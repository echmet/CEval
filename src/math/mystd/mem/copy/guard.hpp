#ifndef MYSTD_MEM_COPY_GUARD_HPP
#define MYSTD_MEM_COPY_GUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------
// Notes

/* $ 1 > We do NOT call AdvanceForRelease after rawcopy in order to release from lp 
 *        if rawcopy does not fully succeeds,
 *        we have to release from p + sz anyway
 *       BUT in order to synchronize lp and loff in case of lp is used
 *        remember that lp is not used only for default destructibles     
 */

//=============================================================================
//-----------------------------------------------------------------------------
// Include

#include <boost\type_traits\has_trivial_assign.hpp>
#include <boost\type_traits\has_nothrow_assign.hpp>
#include <boost\type_traits\detail\ice_and.hpp>

#ifndef MYSTD_MEM_BASE_HPP
#  include "_SHARED\MyStd\mem\base.hpp"
#endif

#ifndef MYSTD_MEM_COPY_GUARD_H
#  include "_SHARED\MyStd\mem\copy\guard.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------
// Definitions

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy(
  typename Allocator::const_reference t, 
  Guard<T, Allocator> &               g, 
  Allocator                           a
)
{
  return 
    inside::copy_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(t, g, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy( 
  typename Allocator::const_pointer   src,
  Guard<T, Allocator> &               g, 
  Allocator                           a
)
{
  return 
    inside::copy_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(src, g, a)
  ;

}
//-----------------------------------------------------------------------------

// ---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy(
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_reference t, 
  Guard<T, Allocator> &               g, 
  Allocator                           a
)
{
  return 
    inside::copy_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(std::nothrow, t, g, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy( 
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_pointer   src,
  Guard<T, Allocator> &               g, 
  Allocator                           a
)
{
  return 
    inside::copy_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_assign<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::copy(std::nothrow, src, g, a)
  ;

}


//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_assignables
template<typename T, typename Allocator>
struct copy_guard_helper<T, Allocator, true>
{

  // reference, throw
  static inline Guard<T, Allocator> & 
  copy( 
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g, 
    Allocator                           a
  )
  {

    copy(std::nothrow, t, g, a);    
    if (!g) throw MYSTD_TMPERR;
    
    return g;  

  }
  
  // field, throw
  static inline Guard<T, Allocator> & 
  copy( 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g, 
    Allocator                         a
  )
  {

    copy(std::nothrow, src, g, a);    
    if (!g) throw MYSTD_TMPERR;
    
    return g;  

  }
  
  // reference, nothrow
  static inline Guard<T, Allocator> & 
  copy(
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g, 
    Allocator                           a
  )
  {

    g.loff = allocator_extensions<Allocator>::rawcopy(t, g.p, g.sz, a);
    AdvanceForRelease(g.lp, g.loff)                                      // $ 1
    
    return g;  

  }
  
  // field, nothrow
  static inline Guard<T, Allocator> & 
  copy( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g, 
    Allocator                         a
  )
  {

    g.loff = allocator_extensions<Allocator>::rawcopy(src, g.p, g.sz, a);
    AdvanceForRelease(g.lp, g.loff)                                      // $ 1
    
    return g;  

  }
  
  
};

//-----------------------------------------------------------------------------
// non-default_copyables
template<typename T, typename Allocator>
struct copy_guard_helper<T, Allocator, false>
{

  // reference, throw
  static inline Guard<T, Allocator> & 
  copy( 
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g, 
    Allocator                           myStd_dummy
  )
  {
    for (g.Reset(); !g; ++g ) *(g.lp) = t; 
    return g;  
  }
  
  // field, throw
  static inline Guard<T, Allocator> & 
  copy( 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g, 
    Allocator                         myStd_dummy
  )
  {
    for (g.Reset(); !g; ++g, ++src ) *(g.lp) = *src; 
    return g;  
  }
  
  // reference, nothrow
  static inline Guard<T, Allocator> & 
  copy(
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g, 
    Allocator                           a
  )
  {
    return copy_guard_nothrow_helper<
      T, Allocator,
      boost::has_nothrow_assign<typename Allocator::value_type>::value
    >::copy(t, g, a);
  }
  
  // field, nothrow
  static inline Guard<T, Allocator> & 
  copy( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g, 
    Allocator                         a
  )
  {
    return copy_guard_nothrow_helper<
      T, Allocator,
      boost::has_nothrow_assign<typename Allocator::value_type>::value
    >::copy(src, g, a);
  }
  
  
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// non-default_assignables, nothrow helper, nothrow_assignables
template<typename T, typename Allocator>
struct copy_guard_nothrow_helper<T, Allocator, true>
{

  // reference, nothrow
  static inline Guard<T, Allocator> & 
  copy(
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g, 
    Allocator                           a
  )
  {
    return copy_guard_helper<T, Allocator, false>::copy(t, g, a);
  }
  
  // field, nothrow
  static inline Guard<T, Allocator> & 
  copy( 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g, 
    Allocator                         a
  )
  {
    return copy_guard_helper<T, Allocator, false>::copy(src, g, a);
  }
  
  
};

//-----------------------------------------------------------------------------
// non-default_assignables, nothrow helper, throw_assignables
template<typename T, typename Allocator>
struct copy_guard_nothrow_helper<T, Allocator, false>
{

  // reference, nothrow
  static inline Guard<T, Allocator> & 
  copy(
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g, 
    Allocator                           a
  )
  {
    try        { return copy_guard_helper<T, Allocator, false>::copy(t, g, a); }
    catch(...) { return g;}
  }
  
  // field, nothrow
  static inline Guard<T, Allocator> & 
  copy( 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g, 
    Allocator                         a
  )
  {
    try        { return copy_guard_helper<T, Allocator, false>::copy(src, g, a); }
    catch(...) { return g;}
  }
  
};

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

