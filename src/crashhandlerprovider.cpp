#include "crashhandlerprovider.h"
#include "crashhandlerwindows.h"

AbstractCrashHandler * CrashHandlerProvider::s_handler(nullptr);

AbstractCrashHandler * CrashHandlerProvider::handler()
{
  return s_handler;
}

bool CrashHandlerProvider::installCrashHandler()
{
#ifdef Q_OS_WIN
  s_handler = new CrashHandlerWindows();
#else
  s_handler = new NullCrashHandler();
#endif // Q_OS

  if (s_handler == nullptr)
    return false;

  return s_handler->install();
}

void CrashHandlerProvider::uninstallCrashHandler()
{
  if (s_handler == nullptr)
    return;

  s_handler->uninstall();
  delete s_handler;
  s_handler = nullptr;
}

