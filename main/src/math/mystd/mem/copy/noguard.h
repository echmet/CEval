#ifndef MYSTD_MEM_COPY_NOGUARD_H
#define MYSTD_MEM_COPY_NOGUARD_H

//=============================================================================
//-----------------------------------------------------------------------------

// Notes:

/* Result : 
 * number of successfully coppied objects     
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
inline typename Allocator::size_type
Copy(
	typename Allocator::const_reference,
	typename Allocator::pointer        ,
	typename Allocator::size_type      ,
	Allocator                          = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type
Copy(
	typename Allocator::const_pointer,
	typename Allocator::pointer      ,
	typename Allocator::size_type    ,
	Allocator                        = Allocator()
);

// ---

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type
Copy(
	std::nothrow_t                     ,
	typename Allocator::const_reference,
	typename Allocator::pointer        ,
	typename Allocator::size_type      ,
	Allocator                          = Allocator()
);

//-----------------------------------------------------------------------------
template<typename T, typename Allocator>
inline typename Allocator::size_type Copy(
	std::nothrow_t                   ,
	typename Allocator::const_pointer,
	typename Allocator::pointer      ,
	typename Allocator::size_type    ,
	Allocator                        = Allocator()
);

//=============================================================================
//-----------------------------------------------------------------------------

namespace inside {

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b>
struct copy_noguard_helper;

//-----------------------------------------------------------------------------
template<typename T, typename Allocator, bool b>
struct copy_noguard_nothrow_helper;

} // namespace

//=============================================================================
//-----------------------------------------------------------------------------

} // namespace

} // namespace

#endif //header
