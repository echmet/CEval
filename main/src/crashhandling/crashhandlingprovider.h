#ifndef CRASHHANDLINGPROVIDER_H
#define CRASHHANDLINGPROVIDER_H

#include "crashhandlerbase.h"
#include <assert.h>
#include <type_traits>

template <typename CrashHandlerType>
class CrashHandlingProvider
{
  static_assert(std::is_base_of<CrashHandlerBase, CrashHandlerType>::value, "CrashHandlerType must be of type CrashHandlerBase");

public:
  CrashHandlingProvider() = delete;
  static void deinitialize()
  {
    if (s_handler != nullptr)
      s_handler->uninstall();

    delete s_handler;
    s_handler = nullptr;
  }

  static bool initialize(const std::string & miniDumpPath)
  {
    if (s_handler != nullptr)
      return true;

    try {
      s_handler = new CrashHandlerType(miniDumpPath);
    } catch (std::bad_alloc &) {
      return false;
    }

    if (!s_handler->install()) {
      delete s_handler;
      s_handler = nullptr;
      return false;
    }

    return true;
  }

  static CrashHandlerType * handler()
  {
    return s_handler;
  }

private:
  static CrashHandlerType *s_handler;

};

template<typename CrashHandlerType> CrashHandlerType * CrashHandlingProvider<CrashHandlerType>::s_handler = nullptr;

#endif // CRASHHANDLINGPROVIDER_H
