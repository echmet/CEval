#ifndef MYSTD_MATH_BASICS_HPP
#define MYSTD_MATH_BASICS_HPP

//===========================================================================
//---------------------------------------------------------------------------

// Includes:

#ifndef MYSTD_MATH_BASICS_H
# include "_SHARED\MyStd\math_basics.h"
#endif 

//===========================================================================
//---------------------------------------------------------------------------

// Definitions:

namespace myStd {

//---------------------------------------------------------------------------
// the type is ready to be rewritten (possible shared resources reallocation)
inline template<typename T> T & invalidate(T & t) 
{ 
  return t;
}

//---------------------------------------------------------------------------
// the type is a unique copy of itself (possible shared resources reallocation and copy)
inline template<typename T> T & seclude(T & t) 
{ 
  return t;
}

//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V> 
T & addmodify(T & result, const U & left, const V & right)
{
  return result = left + right;
}
 
//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V> 
T & submodify(T & result, const U & left, const V & right)
{
	return result = left - right;
}

//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V>
T & multmodify(T & result, const U & left, const V & right)
{
	return result = left * right;
}

//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V>
T & divmodify(T & result, const U & left, const V & right)
{
	return result = left / right;
}

} // namespace

//---------------------------------------------------------------------------
#endif // Header
