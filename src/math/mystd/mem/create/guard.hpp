#ifndef MYSTD_MEM_CREATE_GUARD_HPP
#define MYSTD_MEM_CREATE_GUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------
// Notes

/*
 * $ 1 > If T has a trivial constructor but does not have a trivial destructor
 *       lp must be advaced to p + sz or otherwise the guard would be in
 *       an ill-defined state for later Release call.
 *       If T has both constructor and destructor trivial, 
 *       g.lp == g.p is unused.
 *       
 *       We only call std::advance if really necessary (trivial ctor, nontrivial dtor)
 *       as this puts another constrains to Allocator::pointer and Allocator::size_type,
 *       namely pointer be an Iterator concept and size_type be convertible
 *       to the related Iterator::difference_type.     
 */     

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------
// Includes

#include <boost\type_traits\has_trivial_constructor.hpp>
#include <boost\type_traits\has_nothrow_constructor.hpp>
#include <boost\type_traits\has_trivial_copy.hpp>
#include <boost\type_traits\has_nothrow_copy.hpp>
#include <boost\type_traits\detail\ice_and.hpp>

#ifndef MYSTD_MEM_BASE_HPP
#  include "_SHARED\MyStd\mem\base.hpp"
#endif

#ifndef MYSTD_MEM_RELEASE_GUARD_HPP
#  include "_SHARED\MyStd\mem\release\guard.hpp"
#endif

#ifndef MYSTD_MEM_CREATE_GUARD_H
#  include "_SHARED\MyStd\mem\create\guard.h"
#endif

//=============================================================================
//-----------------------------------------------------------------------------
// Definitions

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------

// > throw >

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create( 
  Guard<T, Allocator> & g,
  Allocator             a 
)
{
  g.lp = g.p = Alloc<T, Allocator>(g.sz, g.p, a);

  return 
    inside::create_guard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(g, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create( 
  typename Allocator::const_reference t,
  Guard<T, Allocator> &               g, 
  Allocator                           a 
)
{
  g.lp = g.p = Alloc<T, Allocator>(g.sz, g.p, a);

  return 
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(t, g, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  typename Allocator::const_pointer src,
  Guard<T, Allocator> &             g, 
  Allocator                         a 
)
{
  g.lp = g.p = Alloc<T, Allocator>(g.sz, g.p, a);
  
  return
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(src, g, a)
  ;

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < throw <

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > nothrow >

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  std::nothrow_t        myStd_dummy,
  Guard<T, Allocator> & g,
  Allocator             a 
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p, a);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(std::nothrow, g, a)
    
    :
  
    g
  
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &\
Create(
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_reference t,
  Guard<T, Allocator> &               g,
  Allocator                           a 
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p, a);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(std::nothrow, t, g, a)
    
    :
  
    g
  
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Create(
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_pointer   src,
  Guard<T, Allocator> &               g,
  Allocator                           a 
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p, a);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(std::nothrow, src, g, a)
    
    :
  
    g
  
  ;

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < nothrow <

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_copyables
template<typename T, typename Allocator>
struct create_guard_helper<T, Allocator, true>
{

  // noinit, throw (does not throw, anyway)
  static inline Guard<T, Allocator> & 
  construct( Guard<T, Allocator> & g, Allocator myStd_dummy)
  { 
    g.loff = g.sz;
    g.lp   = AdvanceForRelease<T, Allocator>(g.lp, g.loff);             // $ 1

    return g; 
  }
  
  // T init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_reference t, Guard<T, Allocator> & g, Allocator a)
  { 

    g.loff = allocator_extensions<Allocator>::rawcopy(t, g.p, g.sz, a);
    g.lp   = AdvanceForRelease<T, Allocator>(g.lp, g.loff);             // $ 1

    if (g.loff != g.sz) throw MYSTD_TMPERR;
    
    return g;                                                            

  }
  
  // src init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_pointer src, Guard<T, Allocator> & g, Allocator a)
  {

    g.loff = allocator_extensions<Allocator>::rawcopy(src, g.p, g.sz, a);
    g.lp   = AdvanceForRelease<T, Allocator>(g.lp, g.loff);             // $ 1

    if (g.loff != g.sz) throw MYSTD_TMPERR;
    
    return g;

  }
  
  //---
  
  // noinit, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g,
    Allocator             a 
  )
  {
    return construct(g, a);
  }
  
  // T init, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g,
    Allocator                           a 
  )
  { 

    g.loff = allocator_extensions<Allocator>::rawcopy(t, g.p, g.sz, a);
    g.lp   = AdvanceForRelease<T, Allocator>(g.lp, g.loff);             // $ 1

    if (g.loff != g.sz) Release<T, Allocator>(std::nothrow, g, a);
    
    return g;                                                            

  }
  
  // src init, nothrow
  static inline Guard<T, Allocator> & 
  construct(
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g,
    Allocator                         a 
  )
  {

    g.loff = allocator_extensions<Allocator>::rawcopy(src, g.p, g.sz, a);
    g.lp   = AdvanceForRelease<T, Allocator>(g.lp, g.loff);             // $ 1

    if (g.loff != g.sz) Release<T, Allocator>(std::nothrow, g, a);
    
    return g;                                                            

  }
  
};

//-----------------------------------------------------------------------------
// non-default_copyables
template<typename T, typename Allocator>
struct create_guard_helper<T, Allocator, false>
{

  // noinit, throw
  static inline Guard<T, Allocator> & 
  construct( Guard<T, Allocator> & g, Allocator a)
  {
    return construct(T(), g, a); 
  }

  // T init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_reference t, Guard<T, Allocator> & g, Allocator a)
  { 
    for (g.loff = 0; !g; ++g) a.construct(g.lp, t);
    return g;
  }
  
  // src init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_pointer src, Guard<T, Allocator> & g, Allocator a)
  {
    for (g.loff = 0; !g; ++g, ++src) a.construct(g.lp, *src);
    return g; 
  }
  
  // ---
  
  // noinit, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g,
    Allocator             a
  )
  {
    return create_guard_nothrow_helper<
      boost::has_nothrow_constructor<typename Allocator::value_type>::value
    >::construct(g, a); 
  }

  // T init, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t,
    Guard<T, Allocator> &               g,
    Allocator                           a
  )
  {
    return create_guard_nothrow_helper<
      boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
    >::construct(t, g, a); 
  }
  
  // src init, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src,
    Guard<T, Allocator> &             g,
    Allocator                         a
  )
  {
    return create_guard_nothrow_helper<
      boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
    >::construct(src, g, a); 
  }
  
};

//-----------------------------------------------------------------------------
// non-default_copyables, nothrow, nothrow_copyables
template<typename T, typename Allocator>
struct create_guard_nothrow_helper<T, Allocator, true>
{

  // noinit
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    Guard<T, Allocator> &               g,
    Allocator                           a
  )
  { 
    return construct(T(), g, a);
  }
  
  // T init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t,
    Guard<T, Allocator> &               g,
    Allocator                           a
  )
  { 
    return create_guard_helper<T, Allocator, false>::construct(t, g, a);
  }
  
  // src init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src,
    Guard<T, Allocator> &             g,
    Allocator                         a
  )
  {
    return create_guard_helper<T, Allocator, false>::construct(src, g, a);
  }
  
};

//-----------------------------------------------------------------------------
// non-default_constructibles, nothrow, throw_copyables
template<typename T, typename Allocator>
struct create_guard_nothrow_helper<T, Allocator, false>
{

  // noinit
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    Guard<T, Allocator> &               g,
    Allocator                           a
  )
  { 
    try        { return Construct<T, Allocator>(T(), g, a); }
    catch(...) { return allocator_extensions<Allocator>::null_type(); }
  }

  // T init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t,
    Guard<T, Allocator> &               g,
    Allocator                           a
  )
  { 
    try        { return create_guard_nothrow_helper<T, Allocator, true>::construct(t, g, a); }
    catch(...) { return Release<T, Allocator>(g, a);   }
  }
  
  // src init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src,
    Guard<T, Allocator> &             g,
    Allocator                         a
  )
  {
    try        { return create_guard_nothrow_helper<T, Allocator, true>::construct(src, g, a); }
    catch(...) { return Release<T, Allocator>(g, a);   }
  }
  
};
//-----------------------------------------------------------------------------
  
} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

