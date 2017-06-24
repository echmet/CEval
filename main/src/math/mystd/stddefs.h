#ifndef MYSTD_STDDEFS_H
#define MYSTD_STDDEFS_H

//#include <_stddef>             // standard definitions such as std::size_t etc.
#include <stddef.h>              // standard definitions such as std::size_t etc.
#include <boost\config.hpp>      // standard boost configs and definitions
#include <boost\type_traits.hpp> // espec. true_type / false_type
#include <new>                   // espec. nothrow_t

// these macros are intended as "informators" (no effect)
#define myStd_virtual
#define myStd_private
#define myStd_protected
#define myStd_inherited   // information that a virtual func is inherited
#define myStd_public
#define myStd_global
#define myStd_macro
#define myStd_dummy       // used for a "dummy" function parameter that is never used
#define myStd_introduce   // used for a template that is only intended to be specialised
#define myStd_interface   // to declare that a class is aimed to be an interface

// defining this $true$ will open disabled sections of code...
// (often causing a compile-time error)
#define MYSTD_DISABLED                 false

// compiler-specific settings
#define myStd_fastcall                 __fastcall
#define myStd_STLcall                  _STL_CALL

// $streamsize$ etc. are declared in $std$ scope in mingw 
// while they are declared in $std::ios$ scope in C++ Builder...
#define MYSTD_STREAMSZE                std::streamsize
#define MYSTD_STREAMPOS                std::streampos
#define MYSTD_STREAMOFF                std::streamoff

namespace myStd {

  typedef boost::true_type  true_type;
  typedef boost::false_type false_type;
  
  template<typename T, T val> 
  struct const_value : boost::integral_constant<T, val> {};

}

#endif // header
