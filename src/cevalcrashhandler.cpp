#include "cevalcrashhandler.h"
#include "crashevent.h"
#include "crasheventcatcher.h"
#include "crashhandling/crashhandlingprovider.h"
#include "crashhandling/crashhandlerwindows.h"
#include <fstream>
#include <QApplication>

#ifdef Q_OS_WIN
  #define CrashHandlerPlatform CrashHandlerWindows
#elif defined (Q_OS_LINUX)
  #define CrashHandlerPlatform CrashHandlerLinux
#else
  #define CrashHandlerPlatform NullCrashHandler
#endif // Q_OS_

class UICrashFinalizer : public CrashHandlerFinalizer<CrashHandlerPlatform>
{
public:
  explicit UICrashFinalizer(CrashHandlerPlatform *handler) :
    CrashHandlerFinalizer(handler)
  {}

  virtual void operator()() override
  {
    CrashEvent *evt = new CrashEvent(m_handler);

    if (m_handler->mainThreadCrashed()) {
      /* Let then know that I have been stiff
         and smiling 'till the crash */

      CEvalCrashHandler::s_catcher->executeEmergency();

      std::ofstream textDump("crashdump.txt", std::ios_base::out);
      if (textDump.is_open()) {
        textDump << m_handler->crashInfo();
        textDump.close();
      }

      /* And now make the final splash... */
      return;
    }

    qApp->postEvent(qApp, evt);

    m_handler->waitForKill();
  }
};

UICrashFinalizer * CEvalCrashHandler::s_uiFinalizer(nullptr);
CrashEventCatcher * CEvalCrashHandler::s_catcher(nullptr);

CrashHandlerBase * CEvalCrashHandler::handler()
{
  return CrashHandlingProvider<CrashHandlerPlatform>::handler();
}

bool CEvalCrashHandler::installCrashHandler()
{
  if (!CrashHandlingProvider<CrashHandlerPlatform>::initialize("CEval_minidump.mdmp"))
    return false;

  try {
    s_uiFinalizer = new UICrashFinalizer(CrashHandlingProvider<CrashHandlerPlatform>::handler());
  } catch (std::bad_alloc &) {
    CrashHandlingProvider<CrashHandlerPlatform>::deinitialize();
    return false;
  }

  try {
    s_catcher = new CrashEventCatcher();
  } catch (std::bad_alloc &) {
    CrashHandlingProvider<CrashHandlerPlatform>::deinitialize();
    delete s_uiFinalizer;
    return false;
  }

  qApp->installEventFilter(s_catcher);

  return true;
}

void CEvalCrashHandler::uninstallCrashHandler()
{
  if (s_catcher != nullptr)
    qApp->removeEventFilter(s_catcher);

  CrashHandlingProvider<CrashHandlerPlatform>::deinitialize();

  delete s_uiFinalizer;
  s_uiFinalizer = nullptr;
  s_catcher = nullptr;
}
