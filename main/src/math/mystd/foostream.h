#ifndef FOOSTREAM_H
#define FOOSTREAM_H

#include <ostream>

// ------------------------------------------------------------------
namespace echmet {

// ------------------------------------------------------------------
class foostream {
public:
  foostream() {}

  foostream(const char *, std::ios::openmode) {}

  void open(const char *, std::ios::openmode) {}

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


