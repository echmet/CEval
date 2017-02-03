#include "cevalcrashhandler.h"
#include "crashevent.h"
#include "crasheventcatcher.h"
#include "crashhandling/crashhandlingprovider.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <QApplication>

#ifdef Q_OS_WIN
  #include "crashhandling/crashhandlerwindows.h"
  #define CrashHandlerPlatform CrashHandlerWindows
#elif defined (Q_OS_LINUX)
  #include "crashhandling/nullcrashhandler.h"
  #define CrashHandlerPlatform NullCrashHandler
#else
  #include "crashhandling/nullcrashhandler.h"
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

      std::ofstream textDump(CEvalCrashHandler::s_textCrashDumpFile.c_str(), std::ios_base::out);
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
const std::string CEvalCrashHandler::s_textCrashDumpFile("CEval_crashDump.txt");

void CEvalCrashHandler::checkForCrash()
{
  std::ifstream textDump(s_textCrashDumpFile.c_str(), std::ios_base::in);
  if (!textDump.good())
    return;

  std::ostringstream textDumpSt;

  textDumpSt << textDump.rdbuf();

  CrashEventCatcher::displayCrashDialog(textDumpSt.str().c_str(), true);
  textDump.close();

  ::remove(s_textCrashDumpFile.c_str());
}

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
