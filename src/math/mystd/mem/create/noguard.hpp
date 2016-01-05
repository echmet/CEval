#ifndef MYSTD_MEM_CREATE_NOGUARD_HPP
#define MYSTD_MEM_CREATE_NOGUARD_HPP

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:

/* $ 1 > All non-guarded create_helpers are automatically nothrow versions
 *       as otherwise there would be no way to safely destroy all
 *       possible objects that have been created in a field 
 *       prior the object causing an exception.
 */ 

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

#ifndef MYSTD_MEM_RELEASE_NOGUARD_HPP
#  include "_SHARED\MyStd\mem\release\noguard.hpp"
#endif

#ifndef MYSTD_MEM_CREATE_NOGUARD_H
#  include "_SHARED\MyStd\mem\create\noguard.h"
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
typename Allocator::pointer Create(
  typename Allocator::size_type sz,
  Allocator                     a,
  typename Allocator::pointer   hint
)
{

  hint = Alloc<T, Allocator>(sz, a, hint);

  return
    inside::create_noguard_helper<
			T, Allocator,
			boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(hint, sz, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  typename Allocator::const_reference t,
  typename Allocator::size_type       sz,
  Allocator                           a,
  typename Allocator::pointer         hint
)
{

  hint = Alloc<T, Allocator>(sz, a, hint);

  return
    inside::create_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(hint, sz, t, a)
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  typename Allocator::const_pointer   src,
  typename Allocator::size_type       sz,
  Allocator                           a,
  typename Allocator::pointer         hint
)
{

  hint = Alloc<T, Allocator>(sz, a, hint);

  return
    inside::create_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(hint, sz, src, a)
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
typename Allocator::pointer Create(
  std::nothrow_t                myStd_dummy,
  typename Allocator::size_type sz,
  Allocator                     a,
  typename Allocator::pointer   hint
)
{

  return 
    ( hint = Alloc<T, Allocator>(std::nothrow, sz, a, hint) )
    
    ? 

    inside::create_noguard_helper<
      T, Allocator,
      boost::has_trivial_constructor<typename Allocator::value_type>::value
    >::construct(std::nothrow, hint, sz, hint)

    :

    hint
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_reference t,
  typename Allocator::size_type       sz,
  Allocator                           a,
  typename Allocator::pointer         hint
)
{

  return 
    ( hint = Alloc<T, Allocator>(std::nothrow, sz, a, hint) )
    
    ? 

    inside::create_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(std::nothrow, hint, sz, t, a)

    :

    hint
  ;

}

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
typename Allocator::pointer Create(
  std::nothrow_t                      myStd_dummy,
  typename Allocator::const_pointer   src,
  typename Allocator::size_type       sz,
  Allocator                           a,
  typename Allocator::pointer         hint
)
{

  return 
    ( hint = Alloc<T, Allocator>(std::nothrow, sz, a, hint) )
    
    ? 

    inside::create_noguard_helper<
      T, Allocator,
      boost::ice_and<
        boost::has_trivial_copy_constructor<typename Allocator::value_type>::value,
        allocator_extensions<Allocator>::is_specialized::value
      >::value
    >::construct(std::nothrow, hint, sz, src, a)

    :

    hint
  ;

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// < nothrow < 

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

// $ 1

//-----------------------------------------------------------------------------
// default_constructibles
template<typename T, typename Allocator>
struct create_noguard_helper<T, Allocator, true>
{

  // noinit, throw (does not throw, anyway)
	static inline typename Allocator::pointer
	construct (
		typename Allocator::pointer   p,
		typename Allocator::size_type sz,
		Allocator                     myStd_dummy
	)
	{ return p; }

	// T init, throw
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{
		typename Allocator::size_type loff = allocator_extensions<Allocator>::rawcopy(t, p, sz, a);

		if (loff != sz) {

			Release<T, Allocator>(p, sz, loff, a);

			throw MYSTD_TMPERR;

		}

		return p;
    
  }
  
  // src init, throw
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer       p,
		typename Allocator::size_type     sz,
		typename Allocator::const_pointer src,
		Allocator                         a
	)
	{
		typename Allocator::size_type loff = allocator_extensions<Allocator>::rawcopy(src, p, sz, a);

		if (loff != sz) {

			Release<T, Allocator>(p, sz, loff, a);

			throw MYSTD_TMPERR;

		}

		return p;
    
  }
  
  // ---

	// noinit, nothrow
	static inline typename Allocator::pointer
  construct (
    std::nothrow_t                myStd_dummy,
    typename Allocator::pointer   p,
    typename Allocator::size_type sz,
    Allocator                     myStd_dummy 
  ) 
  { return p; }
  
  // T init, nothrow
	static inline typename Allocator::pointer
	construct(
		std::nothrow_t                      myStd_dummy,
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{
		typename Allocator::size_type loff = allocator_extensions<Allocator>::rawcopy(t, p, sz, a);

		if (loff != sz) { p = Release<T, Allocator>(p, sz, loff, a); }

		return p;

	}

	// src init, nothrow
	static inline typename Allocator::pointer
  construct(
    std::nothrow_t                    myStd_dummy,
    typename Allocator::pointer       p, 
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src,
    Allocator                         a 
  )
  {
    typename Allocator::size_type loff = allocator_extensions<Allocator>::rawcopy(src, p, sz, a);
    
    if (loff != sz) { p = Release<T, Allocator>(p, sz, loff, a); }
    
    return p;
    
  }
  
};

//-----------------------------------------------------------------------------
// non-default_constructibles
template<typename T, typename Allocator>
struct create_noguard_helper<T, Allocator, false>
{

	// noinit, throw
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer   p,
		typename Allocator::size_type sz,
		Allocator                     a
	)
	{
		return create_noguard_throw_helper<
			boost::has_nothrow_constructor<typename Allocator::value_type>::value
		>::create(p, sz, a);
	}

	// T init, throw
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{
		return create_noguard_throw_helper<
			boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
		>::create(p, sz, t, a);
	}

	// src init, throw
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer       p,
		typename Allocator::size_type     sz,
		typename Allocator::const_pointer src,
		Allocator                         a
	)
	{
		return create_noguard_throw_helper<
			boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
		>::create(p, sz, src, a);
	}

	// ---

	// noinit, nothrow
	static inline typename Allocator::pointer
	construct(
		std::nothrow_t                myStd_dummy,
		typename Allocator::pointer   p,
		typename Allocator::size_type sz,
		Allocator                     a
	)
	{
		return create_noguard_nothrow_helper<
			boost::has_nothrow_constructor<typename Allocator::value_type>::value
		>::construct(p, sz, a);
	}

	// T init, nothrow
	static inline typename Allocator::pointer
	construct(
		std::nothrow_t                      myStd_dummy,
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{
		return create_noguard_nothrow_helper<
			boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
		>::construct(p, sz, t, a);
	}

	// src init, nothrow
	static inline typename Allocator::pointer
  construct( 
    std::nothrow_t                    myStd_dummy,
    typename Allocator::pointer       p,
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src,
    Allocator                         a 
  )
  {
    return create_noguard_nothrow_helper<
      boost::has_nothrow_copy_constructor<typename Allocator::value_type>::value
    >::construct(p, sz, src, a); 
  }
  
};
//-----------------------------------------------------------------------------
  
// ---

//-----------------------------------------------------------------------------
// non-default_constructibles, throw, nothrow_copyables
template<typename T, typename Allocator>
struct create_noguard_throw_helper<T, Allocator, true>
{

	// noinit
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		Allocator                           a
	)
	{
		return create_noguard_nothrow_helper<T, Allocator, true>::create(p, sz, a);
	}


	// T init
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{
		return create_noguard_nothrow_helper<T, Allocator, true>::create(p, sz, t, a);
	}

	// src init
	static inline typename Allocator::pointer
	construct(
    typename Allocator::pointer       p,
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src,
    Allocator                         a 
  )
  {
    return create_noguard_nothrow_helper<T, Allocator, true>::create(p, sz, src, a);
  }
  
};

//-----------------------------------------------------------------------------
// non-default_constructibles, throw, throw_copyables
template<typename T, typename Allocator>
struct create_noguard_throw_helper<T, Allocator, false>
{

  // noinit
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		Allocator                           a
	)
	{
		return create(p, sz, T(), a);
	}


	// T init
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{

		Allocator::size_type loff = 0;
		Allocator::pointer   lp   = p;

		try        { for (; loff != sz; ++loff, ++lp) a.construct(lp, t); }
		catch(...) { Release<T, Allocator>(p, sz, lp, loff, a); throw; }

		return p;

	}

	// src init
	static inline typename Allocator::pointer
  construct( 
    typename Allocator::pointer       p,
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src,
    Allocator                         a 
  )
  {

    Allocator::size_type loff = 0;
    Allocator::pointer   lp   = p;

    try        { for (; loff != sz; ++loff, ++lp, ++src) a.construct(lp, *src); }
    catch(...) { Release<T, Allocator>(p, sz, lp, loff, a); throw; }

    return p; 

  }
  
};
//-----------------------------------------------------------------------------

// ---

//-----------------------------------------------------------------------------
// non-default_constructibles, nothrow, nothrow_copyables
template<typename T, typename Allocator>
struct create_noguard_nothrow_helper<T, Allocator, true>
{

  // noinit
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		Allocator                           a
	)
	{
		return create(p, sz, T(), a);
	}


	// T init
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{

		Allocator::size_type loff = 0;
		Allocator::pointer   lp   = p;

		for (; loff != sz; ++loff, ++lp) a.construct(lp, t);

		return p;

	}

	// src init
	static inline typename Allocator::pointer
  construct( 
    typename Allocator::pointer       p,
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src,
    Allocator                         a 
  )
  {

    Allocator::size_type loff = 0;
    Allocator::pointer   lp   = p;

    for (; loff != sz; ++loff, ++lp, ++src) a.construct(lp, *src);

    return p; 

  }
  
};

//-----------------------------------------------------------------------------
// non-default_constructibles, nothrow, throw_copyables
template<typename T, typename Allocator>
struct create_noguard_nothrow_helper<T, Allocator, false>
{

  // noinit
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		Allocator                           a
	)
	{
		try        { return Create<T, Allocator>(p, sz, T(), a); }
		catch(...) { return allocator_extensions<Allocator>::null_type(); }
	}

	// T init
	static inline typename Allocator::pointer
	construct(
		typename Allocator::pointer         p,
		typename Allocator::size_type       sz,
		typename Allocator::const_reference t,
		Allocator                           a
	)
	{

		Allocator::size_type loff = 0;
		Allocator::pointer   lp   = p;

		try        { for (; loff != sz; ++loff, ++lp) a.construct(lp, t); }
		catch(...) { p = Release<T, Allocator>(p, sz, lp, loff, a); }

		return p;

	}

	// src init
  static inline typename Allocator::pointer
  construct( 
    typename Allocator::pointer       p,
    typename Allocator::size_type     sz,
    typename Allocator::const_pointer src,
    Allocator                         a 
  )
  {

    Allocator::size_type loff = 0;
    Allocator::pointer   lp   = p;

    try        { for (; loff != sz; ++loff, ++lp, ++src) a.construct(lp, *src); }
    catch(...) { p = Release<T, Allocator>(p, sz, lp, loff, a); }
    
    return p;

  }
  
};
  
} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif // header

