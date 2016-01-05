#ifndef MYSTD_TESTS_H
#define MYSTD_TESTS_H

#include <iostream>

template<typename TF>
inline bool MYSTD_TESTCALL_(TF F, const char * str)
{

  std::cout << "\n=================================\n"
            << str << std::endl
  ;

  return F();

}

#define MYSTD_TESTCALL(F) MYSTD_TESTCALL_(F, #F)

#define MYSTD_TESTCALLRETURN(F) if (!MYSTD_TESTCALL(F)) return false

#define MYSTD_TESTCALLTHROW(F, EX)  if (!MYSTD_TESTCALL(F)) throw EX

inline bool MYSTD_TESTASSERT_(bool b, int line, const char * fc, const char * fl, const char * str)
{

  if (!b) std::cout << "\n ! #" << line << " @ " << fc << " @ " << fl << " > !( " << str << " )" << std::endl;

  return b;

}

#define MYSTD_TESTASSERT(CMD) MYSTD_TESTASSERT_( (CMD), __LINE__, __FUNCTION__, __FILE__, #CMD)

#define MYSTD_TESTASSERTRETURN(CMD) if (!MYSTD_TESTASSERT(CMD)) return false

#define MYSTD_TESTASSERTTHROW(CMD, EX) if (!MYSTD_TESTASSERT(CMD)) throw EX


#endif // MYSTD_TESTS_H
