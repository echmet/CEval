#ifndef SUPPORTEDENCODINGS_H
#define SUPPORTEDENCODINGS_H

#include <string>
#include <vector>

class SupportedEncodings
{
public:
#if defined ENCODING_USE_ICU
  typedef std::string EncodingType;
#elif defined ENCODING_USE_WINAPI
  typedef int EncodingType;
#else
#error "Unsupported encoding conversion method"
#endif // ENCODING_USE_
  typedef std::pair<std::string, EncodingType> Encoding;
  typedef std::vector<Encoding> EncodingsVec;

  static const EncodingsVec & supportedEncodings();

  static const EncodingType INVALID_ENCTYPE;
};

#endif // SUPPORTEDENCODINGS_H
