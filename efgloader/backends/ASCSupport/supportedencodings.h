#ifndef SUPPORTEDENCODINGS_H
#define SUPPORTEDENCODINGS_H

#include <string>
#include <vector>

class SupportedEncodings
{
public:
  typedef std::pair<std::string, std::string> Encoding;
  typedef std::vector<Encoding> EncodingsVec;

  static const EncodingsVec & supportedEncodings();
};

#endif // SUPPORTEDENCODINGS_H
