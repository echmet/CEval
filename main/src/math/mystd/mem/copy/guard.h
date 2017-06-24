#ifndef MYSTD_MEM_COPY_GUARD_H
#define MYSTD_MEM_COPY_GUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:

/* Result : 
 * if !g, an exception arrised in a copy constructor at offset g.loff, address g.lp
 * => All objects up to g.lp are copied     
 * 
 * Preconditions :
 * 
 * Valid Guarded expected
 * - Copy does not check for Guard::operator bool()
 * 
 * ! Warning :
 * If Copy fails, the guard may violate preconditions for later call of Release  
 * => call Copy with a new copy-constructed Guard rather than the original one
 *    as it came from the previous call of Create          
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
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy(
	typename Allocator::const_reference,
	Guard<T, Allocator> &              ,
	Allocator                          = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy(
	typename Allocator::const_pointer,
	Guard<T, Allocator> &            ,
	Allocator                        = Allocator()
);

//---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy(
	std::nothrow_t                     ,
	typename Allocator::const_reference,
	Guard<T, Allocator> &              ,
	Allocator = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline Guard<T, Allocator> &
Copy(
	std::nothrow_t                   ,
	typename Allocator::const_pointer,
	Guard<T, Allocator> &            ,
	Allocator = Allocator()
);

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct copy_guard_helper;

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b> 
struct copy_guard_nothrow_helper;

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif //header
