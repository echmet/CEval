#include "supportedencodings.h"

#if defined ENCODING_USE_ICU

const SupportedEncodings::EncodingType SupportedEncodings::INVALID_ENCTYPE{""};

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

const SupportedEncodings::EncodingType SupportedEncodings::INVALID_ENCTYPE{-1};

const SupportedEncodings::EncodingsVec & SupportedEncodings::supportedEncodings()
{
  static const EncodingsVec encs = {
    {"ISO-8859-1 (Latin 1)", 28591},
    {"ISO-8859-2 (Latin 2)", 28592},
    {"Windows 1250", 1250},
    {"Windows 1251", 1251},
    {"Windows 1252", 1252},
    {"UTF-8", 65001},
    {"UTF-16LE", 1200},
    {"UTF-16BE", 1201},
  };

  return encs;
}

#else
#error "Unknown encoding conversion method"
#endif // ENCODING_USE_
