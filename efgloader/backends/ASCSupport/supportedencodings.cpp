#include "supportedencodings.h"

#if defined ENCODING_USE_ICU

const SupportedEncodings::EncodingsVec & SupportedEncodings::supportedEncodings()
{
  static const EncodingsVec encs = {
    {"ISO-8859-1 (Latin 1)", "ISO-8859-1"},
    {"ISO-8859-2 (Latin 2)", "ISO-8859-2"},
    {"Windows 1250", "windows-1250"},
    {"Windows 1251", "windows-1251"},
    {"Windows 1252", "windows-1252"},
    {"UTF-8", "UTF-8"},
    {"UTF-16LE", "UTF-16LE"},
    {"UTF-16BE", "UTF-16BE"},
  };

  return encs;
}

#elif defined ENCODING_USE_WINAPI

#else
#error "Unknown encoding conversion method"
#endif // ENCODING_USE_
