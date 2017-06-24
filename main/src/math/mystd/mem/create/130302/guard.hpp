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
#include <boost\type_traits\has_trivial_copy_constructor.hpp>
#include <boost\type_traits\has_nothrow_copy_constructor.hpp>
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
  Guard<T, Allocator> & g 
)
{
  g.lp = g.p = Alloc<T, Allocator>(g.sz, g.p);

  return 
    inside::create_guard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(g)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create( 
  typename Allocator::const_reference t,
  Guard<T, Allocator> &               g 
)
{
  g.lp = g.p = Alloc<T, Allocator>(g.sz, g.p);

  return 
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(t, g)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  typename Allocator::const_pointer src,
  Guard<T, Allocator> &             g 
)
{
  g.lp = g.p = Alloc<T, Allocator>(g.sz, g.p);
  
  return 
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(src, g)
  ;

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < throw <

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > single no-throw >

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  std::nothrow_t        myStd_dummy,
  Guard<T, Allocator> & g  
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(g)
    
    :
  
    g
  
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_reference t,
  Guard<T, Allocator> &               g  
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p);

  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(t, g)
    
    :
  
    g
  
  ;
}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator >
inline Guard<T, Allocator> & Create(
  std::nothrow_t                    myStd_dummy,
  typename Allocator::const_pointer src,
  Guard<T, Allocator> &             g
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(src, g)
    
    :
  
    g
  
  ;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < single nothrow >

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > double nothrow >

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  std::nothrow_t        myStd_dummy,
  std::nothrow_t        myStd_dummy,
  Guard<T, Allocator> & g
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(std::nothrow, g)
    
    :
  
    g
  
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  std::nothrow_t                      myStd_dummy,
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_reference t,
  Guard<T, Allocator> &               g
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(std::nothrow, t, g)
    
    :
  
    g
  
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> & Create(
  std::nothrow_t                      myStd_dummy,
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_pointer   src,
  Guard<T, Allocator> &               g
)
{
  g.lp = g.p = Alloc<T, Allocator>(std::nothrow, g.sz, g.p);
  
  return 
    
    g.p 
    
    ?
  
    inside::create_guard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(std::nothrow, src, g)
    
    :
  
    g
  
  ;

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < double nothrow <

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
// default_copyables
template<typename T, typename Allocator>
struct create_guard_helper<T, Allocator, true>
{

  // noinit, throw (does not throw anyway)
  static inline Guard<T, Allocator> & 
  construct( Guard<T, Allocator> & g )
  { 
    g.loff = g.sz;

    AdvanceForRelease(g.lp, g.loff);                                    // $ 1

    return g; 
  }
  
  // T init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_reference t, Guard<T, Allocator> & g )
  { 

    g.loff = allocator_extensions<Allocator>::rawcopy(g.p, g.sz, t);

    AdvanceForRelease(g.lp, g.loff);                                     // $ 1

    if (g.loff != g.sz) throw MYSTD_TMPERR;
    
    return g;                                                            

  }
  
  // src init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_pointer src, Guard<T, Allocator> & g )
  {

    g.loff = allocator_extensions<Allocator>::rawcopy(g.p, g.sz, src);

    AdvanceForRelease(g.lp, g.loff);                                     // $ 1

    if (g.loff != g.sz) throw MYSTD_TMPERR;
    
    return g;

  }
  
  //---
  
  // noinit, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g 
  )
  {
    return construct(g);
  }
  
  // T init, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t, 
    Guard<T, Allocator> &               g
  )
  { 

    g.loff = allocator_extensions<Allocator>::rawcopy(g.p, g.sz, t);

    AdvanceForRelease(g.lp, g.loff);                                     // $ 1

    if (g.loff != g.sz) Release(g);
    
    return g;                                                            

  }
  
  // src init, nothrow
  static inline Guard<T, Allocator> & 
  construct(
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src, 
    Guard<T, Allocator> &             g
  )
  {

    g.loff = allocator_extensions<Allocator>::rawcopy(g.p, g.sz, src);

    AdvanceForRelease(g.lp, g.loff);                                     // $ 1

    if (g.loff != g.sz) Release(g);
    
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
  construct( Guard<T, Allocator> & g )
  {
    return construct(T(), g); 
  }

  // T init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_reference t, Guard<T, Allocator> & g )
  { 
    for (g.loff = 0; !g; ++g) a.construct(g.lp, t);
    return g;
  }
  
  // src init, throw
  static inline Guard<T, Allocator> & 
  construct( typename Allocator::const_pointer src, Guard<T, Allocator> & g )
  {
    for (g.loff = 0; !g; ++g, ++src) a.construct(g.lp, *src);
    return g; 
  }
  
  // ---
  
  // noinit, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g
  )
  {
    construct(std::nothrow_t, T(), g);
  }

  // T init, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t,
    Guard<T, Allocator> &               g
  )
  {
    return create_guard_nothrow_helper<
      boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
    >::construct(t, g); 
  }
  
  // src init, nothrow
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src,
    Guard<T, Allocator> &             g
  )
  {
    return create_guard_nothrow_helper<
      boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
    >::construct(src, g); 
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
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g
  )
  {
    return create_guard_helper<T, Alllocator, false>::construct(g);
  }

  // T init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t,
    Guard<T, Allocator> &               g
  )
  { 
    return create_guard_helper<T, Alllocator, false>::construct(t, g);
  }
  
  // src init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src,
    Guard<T, Allocator> &             g
  )
  {
    return create_guard_helper<T, Alllocator, false>::construct(src, g);
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
    std::nothrow_t        myStd_dummy, 
    Guard<T, Allocator> & g
  )
  {
    try        { return create_guard_helper<T, Allocator, false>::construct(g); }
    catch(...) { return Release(g);   }
  }

  // T init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                      myStd_dummy, 
    typename Allocator::const_reference t,
    Guard<T, Allocator> &               g
  )
  { 
    try        { return create_guard_helper<T, Allocator, false>::construct(t, g); }
    catch(...) { return Release(g);   }
  }
  
  // src init
  static inline Guard<T, Allocator> & 
  construct( 
    std::nothrow_t                    myStd_dummy, 
    typename Allocator::const_pointer src,
    Guard<T, Allocator> &             g
  )
  {
    try        { return create_guard_helper<T, Allocator, false>::construct(src, g); }
    catch(...) { return Release(g);   }
  }
  
};
//-----------------------------------------------------------------------------
  
} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

