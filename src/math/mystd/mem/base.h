#ifndef MYSTD_MEM_BASE_H
#define MYSTD_MEM_BASE_H

//=============================================================================
//-----------------------------------------------------------------------------

// Includes

#include <memory> // std::allocator

#include "_SHARED\MyStd\_MyStdUnit.set"
#include "_SHARED\MyStd\stddefs.h"

//=============================================================================
//-----------------------------------------------------------------------------

// Declarations:

namespace myStd {

namespace mem {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator = std::allocator<T> > class Guard;

template<typename T> myStd_introduce                          class allocator_extensions;
                                                 
//=============================================================================
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Alloc(
	typename Allocator::size_type     = 1,
	Allocator                         = Allocator(),
	typename Allocator::const_pointer = allocator_extensions<Allocator>::null_type()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Alloc(
	std::nothrow_t,
	typename Allocator::size_type     = 1,
	Allocator                         = Allocator(),
	typename Allocator::const_pointer = allocator_extensions<Allocator>::null_type()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Dealloc(
	typename Allocator::pointer,
	typename Allocator::size_type,
	Allocator                      = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::pointer Dealloc(
	std::nothrow_t,
	typename Allocator::pointer,
	typename Allocator::size_type
	Allocator                      = Allocator()
);
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > Guard >

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T, typename Allocator> struct Guard {

	typedef Allocator allocator_type;

	typedef typename Allocator::value_type      value_type;
	typedef typename Allocator::pointer         pointer;
	typedef typename Allocator::reference       reference;
	typedef typename Allocator::const_pointer   const_pointer;
	typedef typename Allocator::const_reference const_reference;
	typedef typename Allocator::size_type       size_type;
	typedef typename Allocator::difference_type difference_type;

  pointer       p;
  size_type     sz;
  size_type     loff;
  pointer       lp;
  
  explicit Guard(
    size_type      _sz,
    pointer        _p = allocator_extensions<Allocator>::null_type()           // 1
  ) : p(_p), sz(_sz), loff(0), lp(allocator_extensions<Allocator>::null_type())
  {}

  Guard(const Guard<Allocator> & other)
  : p(other.p), sz(other.sz), loff(other.loff), lp(other.lp)
  {}
  
  operator bool()                { return loff == sz; }
  bool     operator!()           { return loff != sz; }
  operator pointer()             { return p; }
  Guard<Allocator> & operator++(){ ++loff, ++lp; return *this; }
  
  Guard<Allocator> & Reset()     { loff = 0; lp = p; return *this;  }
};
//-----------------------------------------------------------------------------
// 1 _p is used as the hint for Allocator::allocate(sz, hint)

//-----------------------------------------------------------------------------

// < Guard <

//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// > allocator_extensions>

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// default
template<typename Allocator> 
struct allocator_extensions<Allocator>
{
  typedef typename Allocator::value_type      value_type;
  typedef typename Allocator::pointer         pointer;
  typedef typename Allocator::reference       reference;
  typedef typename Allocator::const_pointer   const_pointer;
  typedef typename Allocator::const_reference const_reference;
  typedef typename Allocator::size_type       size_type;
  typedef typename Allocator::difference_type difference_type;
  
  typedef false_type is_specialized;
  typedef false_type has_nothrow_deallocator;

  struct  null_type { operator pointer() { return NULL; } };  
  
  static inline myStd_introduce pointer allocate(std::nothrow_t, size_type, pointer, Allocator);

  static inline myStd_introduce size_type rawcopy(const_reference, pointer, size_type, Allocator);
  static inline myStd_introduce size_type rawcopy(const_pointer,   pointer, size_type, Allocator);
  
};

//-----------------------------------------------------------------------------
// allocator
template<typename T> 
struct allocator_extensions<std::allocator<T> >
{
	typedef typename std::allocator<T>::value_type      value_type;
	typedef typename std::allocator<T>::pointer         pointer;
	typedef typename std::allocator<T>::reference       reference;
	typedef typename std::allocator<T>::const_pointer   const_pointer;
	typedef typename std::allocator<T>::const_reference const_reference;
	typedef typename std::allocator<T>::size_type       size_type;
	typedef typename std::allocator<T>::difference_type difference_type;

	typedef true_type is_specialized;
	typedef true_type has_nothrow_deallocator;

	struct null_type { operator pointer() { return NULL; } };

	static inline pointer allocate(std::nothrow_t,  size_type, const_pointer, std::allocator<T>);
	static inline size_type rawcopy (const_reference, pointer,   size_type,     std::allocator<T>);
  static inline size_type rawcopy (const_pointer,   pointer,   size_type,     std::allocator<T>);
  
};

//-----------------------------------------------------------------------------

// < allocator_extensions<allocator<T> > <

//----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b>
struct dealloc_helper;

//-----------------------------------------------------------------------------
template<typename T, bool b>
struct allocator_rawcopy_helper;

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

//-----------------------------------------------------------------------------
#endif //header
