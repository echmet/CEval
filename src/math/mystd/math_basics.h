#ifndef MYSTD_MATH_BASICS_H
#define MYSTD_MATH_BASICS_H

//===========================================================================
//---------------------------------------------------------------------------

// Definitions:

namespace myStd {

//---------------------------------------------------------------------------
inline template<typename T> T & invalidate(T & t); 

//---------------------------------------------------------------------------
inline template<typename T> T & seclude(T & t); 

//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V> 
T & addmodify(T & result, const U & left, const V & right);
 
//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V> 
T & submodify(T & result, const U & left, const V & right);

//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V>
T & multmodify(T & result, const U & left, const V & right);

//---------------------------------------------------------------------------
inline template<typename T, typename U, typename V>
T & divmodify(T & result, const U & left, const V & right);

} // namespace

//---------------------------------------------------------------------------
#endif // Header
