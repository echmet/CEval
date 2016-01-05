#ifndef MYSTD_MEM_CREATE_NOGUARD_HPP
#define MYSTD_MEM_CREATE_NOGUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------

#include <boost\type_traits\has_trivial_constructor.hpp>
#include <boost\type_traits\has_trivial_copy_constructor.hpp>
#include <boost\type_traits\detail\ice_and.hpp>

#ifndef MYSTD_MEM_BASE_HPP
#  include "_SHARED\MyStd\mem\base.hpp"
#endif

#ifndef MYSTD_MEM_RELEASE_NOGUARD_HPP
#  include "_SHARED\MyStd\mem\release\noguard.hpp"
#endif

#ifndef MYSTD_MEM_CREATE_NOGUARD_H
#  include "_SHARED\MyStd\mem\create\noguard.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  typename Allocator::size_type sz,
  typename Allocator::pointer   hint
)
{

  return 
    ( hint = Alloc<T, Allocator>(std::nothrow, sz, hint) )
    
    ? 

    inside::create_noguard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(sz, hint)

    :

    hint
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  typename Allocator::const_reference t,
  typename Allocator::size_type       sz,
  typename Allocator::pointer         hint
)
{

  return 
    ( hint = Alloc<T, Allocator>(std::nothrow, sz, hint) )
    
    ? 

    inside::create_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(t, sz, hint)

    :

    hint
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  typename Allocator::const_pointer   src,
  typename Allocator::size_type       sz,
  typename Allocator::pointer         hint
)
{

  return 
    ( hint = Alloc<T, Allocator>(std::nothrow, sz, hint) )
    
    ? 

    inside::create_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(src, sz, hint)

    :

    hint
  ;

}
//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_constructibles
template<typename T, typename Allocator>
struct create_noguard_helper<T, Allocator, true>
{

  // noinit, nothrow
  inline typename Allocator::pointer  
  construct (
    typename Allocator::pointer   p,
    typename Allocator::size_type sz
  ) 
  { return p; }
  
  // T init, nothrow
  inline typename Allocator::pointer  
  construct(
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz
  )
  { 
    typename Allocator::size_type loff = allocator_extensions<Allocator>::rawcopy(p, sz, t);
    
    if (loff != sz) {
      typename Allocator::pointer lp = p;
      p = Release(p, sz, AdvanceForRelease(lp, loff), loff);
    }
    
    return p;
    
  }
  
  // src init, nothrow
  inline typename Allocator::pointer  
  construct(
    typename Allocator::const_pointer src, 
    typename Allocator::pointer       p, 
    typename Allocator::size_type     sz
  )
  {
    typename Allocator::size_type loff = allocator_extensions<Allocator>::rawcopy(src, sz, t);
    
    if (loff != sz) {
      typename Allocator::pointer lp = p;
      p = Release(p, sz, AdvanceForRelease(lp, loff), loff);
    }
    
    return p;
    
  }
  
};

//-----------------------------------------------------------------------------
// non-default_constructibles
template<typename T, typename Allocator>
struct create_guard_helper<T, Allocator, false>
{

  // noinit, nothrow
  inline typename Allocator::pointer  
  construct(
    typename Allocator::pointer   p, 
    typename Allocator::size_type sz
 )
  {
    return construct(T(), p, sz);
  }

  // T init, nothrow
  inline typename Allocator::pointer  
  construct( 
    typename Allocator::const_reference t, 
    typename Allocator::pointer         p, 
    typename Allocator::size_type       sz 
  )
  { 

    Allocator a;
    Allocator::size_type loff = 0;
    Allocator::pointer   lp   = p;
    try {
       
      for (; loff != sz; ++loff, ++lp) a.construct(lp, t);
      return p; 

    }
    catch(...) { return Release(p, sz, lp, loff); }

  }
  
  // src init, nothrow
  inline typename Allocator::pointer  
  construct( 
    typename Allocator::pointer       p,
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src 
  )
  {

    Allocator a;
    Allocator::size_type loff = 0;
    Allocator::pointer   lp   = p;
    try {
       
      for (; loff != sz; ++loff, ++lp, ++src) a.construct(lp, *src);
      return p; 

    }
    catch(...) { return Release(p, sz, lp, loff); }

  }
  
};
//-----------------------------------------------------------------------------
  
} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

