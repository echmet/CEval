#ifndef FOOSTREAM_H
#define FOOSTREAM_H

#include <ostream>

// ------------------------------------------------------------------
namespace echmet {

// ------------------------------------------------------------------
class foostream {
public:
  foostream() {}

  foostream(const char *, std::_Ios_Openmode) {}

  void open(const char *, std::_Ios_Openmode) {}

};

// ------------------------------------------------------------------
template<typename T>
inline foostream &operator<<(foostream & foo, T const &) { return foo; }

// ------------------------------------------------------------------
inline foostream &operator<<(foostream & foo, std::ostream& (*)(std::ostream&))
{
    return foo;
}

// ------------------------------------------------------------------
} // namespace echmet

#endif // FOOSTREAM_H


